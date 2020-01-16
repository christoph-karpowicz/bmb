#include "broker.h"

const char needsQueue[6][12] = {
    BROKER_CONSUME,
    BROKER_PRODUCE,
    BROKER_LENGTH,
    BROKER_GET,
    BROKER_GET_ALL,
    BROKER_REMOVE_QUEUE,
};

const char needsIndex[1][12] = {
    BROKER_GET,
};

const char needsData[3][12] = {
    BROKER_PRODUCE,
    BROKER_LENGTH,
    BROKER_ADD_QUEUE,
};


Broker *broker_init()
{
    Broker *broker = (Broker *) mem_alloc(sizeof(Broker));
    broker->queuePool = queue_pool_init();
    queue_pool_load(broker->queuePool);
    return broker;
}

struct broker_response broker_dispatch(Broker *this, struct broker_request broker_req)
{
    struct broker_response res;
    Queue *queue    = NULL;
    char *errMsg    = NULL;

    // Validate the request.
    bool resNeedsQueue  = in_str_array(broker_req.type, needsQueue, 6);
    bool resNeedsIndex  = in_str_array(broker_req.type, needsIndex, 1);
    bool resNeedsData   = in_str_array(broker_req.type, needsData, 3);

    if (resNeedsQueue) {
        queue = queue_pool_get_by_name(this->queuePool, broker_req.queueName);
        if (broker_req.queueName == NULL) {
            errMsg = "Request is missing the queue name.";
        }
        else if (strcmp(broker_req.queueName, "") == 0) {
            errMsg = "Empty queue name given.";
        }
        else if (queue == NULL && strcmp(broker_req.queueName, "") != 0) {
            errMsg = "Queue not found.";
        }
    }

    if (resNeedsIndex && broker_req.index == 0) {
        errMsg = "Request is missing an index.";
    }

    if (resNeedsData && (broker_req.data == NULL || strcmp(broker_req.data, "") == 0)) {
        errMsg = "Request is missing the data.";
    }

    if (errMsg != NULL) {
        res = create_response(false, 200, errMsg, NULL, NULL);
        return res;
    }
    
    broker_req.queue = queue;
    
    // Dispatch broker action.
    if (strcmp(broker_req.type, BROKER_CONSUME) == 0) {
        res = consume(this, broker_req);
    }
    else if (strcmp(broker_req.type, BROKER_PRODUCE) == 0) {
        res = produce(this, broker_req);
    }
    else if (strcmp(broker_req.type, BROKER_LENGTH) == 0) {
        res = length(this, broker_req);
    }
    else if (strcmp(broker_req.type, BROKER_GET) == 0) {
        res = get(this, broker_req);
    }
    else if (strcmp(broker_req.type, BROKER_GET_ALL) == 0) {
        res = get_all(this, broker_req);
    }
    else if (strcmp(broker_req.type, BROKER_ADD_QUEUE) == 0) {
        res = add_queue(this, broker_req);
    }
    else if (strcmp(broker_req.type, BROKER_GET_ALL_QUEUE_NAMES) == 0) {
        res = get_all_queue_names(this, broker_req);
    }
    else if (strcmp(broker_req.type, BROKER_REMOVE_QUEUE) == 0) {
        res = remove_queue(this, broker_req);
    }

    return res;
}

static struct broker_response add_queue(Broker *this, struct broker_request req)
{
    struct broker_response res;
    unsigned short int code;
    bool success        = false;
    char *msg           = NULL;
    char *errMsg        = NULL;

    Queue *queueDuplicate = queue_pool_get_by_name(this->queuePool, req.data);
    if (queueDuplicate != NULL) {
        msg = "Failed: Queue with given name already exists.";
        goto createResponse;
    }

    if (queue_pool_add(this->queuePool, req.data, false)) {
        success = true;
        msg = "Queue created.";
    }
    else {
        msg = "Failed to create queue.";
    }

    createResponse:
    code = 200;
    
    res = create_response(success, code, msg, NULL, errMsg);

    return res;
}

static struct broker_response consume(Broker *this, struct broker_request req)
{
    struct broker_response res;
    unsigned short int code;
    bool success    = false;
    char *msg       = NULL;
    char *data      = NULL;
    char *errMsg    = NULL;

    if (!_Queue.isEmpty(req.queue)) {
        Node *node = _Queue.poll(req.queue);
        char decodedMessage[strlen(_Node.getMessage(node)) + 1];
        url_decode(_Node.getMessage(node), decodedMessage);

        data = (char *) mem_alloc(strlen(decodedMessage) + 1);
        strcpy(data, decodedMessage);
        _Node.destruct(node);
        success = true;
    }
    else {
        msg = "Queue is empty.";
    }

    code = 200;

    res = create_response(success, code, msg, data, errMsg);
    mem_free(data);

    return res;
}

static struct broker_response create_response(bool success, unsigned short int code, char *message, char *data, char *errorMessage)
{
    struct broker_response res;
    unsigned short int altCode  = 0;
    char *altErrMsg             = NULL;
    cJSON *json_success         = NULL;
    cJSON *json_payload         = NULL;
    cJSON *json_message         = NULL;
    cJSON *json_data            = NULL;

    json_success = cJSON_CreateBool(success);
    if (json_success == NULL)
    {
        altCode = 500;
        altErrMsg = "Failed to add success value to response JSON.";
        goto createResponse;
    }

    json_payload = cJSON_CreateObject();

    if (message != NULL || errorMessage != NULL) {
        json_message = cJSON_CreateString(message != NULL ? message : errorMessage);
        if (json_message == NULL)
        {
            altCode = 500;
            altErrMsg = "Failed to add message value to response JSON.";
            goto createResponse;

        }
        cJSON_AddItemToObject(json_payload, "message", json_message);
    }

    if (data != NULL) {
        json_data = cJSON_CreateString(data);
        if (json_data == NULL)
        {
            altCode = 500;
            altErrMsg = "Failed to add data value to response JSON.";
            goto createResponse;

        }
        cJSON_AddItemToObject(json_payload, "data", json_data);
    }

    createResponse:
    res.success         = json_success;
    res.payload         = json_payload;
    res.code            = altCode > 0 ? altCode : code;
    res.errorMessage    = altErrMsg != NULL ? altErrMsg : errorMessage;

    // printf("data: %s\n", data);

    return res;
}

static struct broker_response get(Broker *this, struct broker_request req)
{
    struct broker_response res;
    unsigned short int code;
    bool success    = false;
    char *msg       = NULL;
    char *data      = NULL;
    char *errMsg    = NULL;

    const int queueSize =   _Queue.size(req.queue);
    if (queueSize > req.index) {
        Node *node = _Queue.get(req.queue, req.index);
        data = (char *) mem_alloc(strlen(_Node.getMessage(node)) + 1);
        strcpy(data, _Node.getMessage(node));
        success = true;
    }
    else {
        msg = "Node index out of range.";
    }
    code = 200;

    res = create_response(success, code, msg, data, errMsg);

    mem_free(data);

    return res;
}

static struct broker_response get_all(Broker *this, struct broker_request req)
{
    struct broker_response res;
    unsigned short int code;
    char *msg               = NULL;
    char *errMsg            = NULL;
    bool success            = false;
    char *data              = NULL;
    const int queueSize     = _Queue.size(req.queue);
    cJSON *nodes            = cJSON_CreateObject();
    cJSON *nodesArray       = NULL;
    char *nodesString       = NULL;
    
    nodesArray = cJSON_AddArrayToObject(nodes, "nodes");
    if (nodesArray == NULL) {
        code = 500;
        errMsg = "Failed to add an array to JSON node list.";
        goto createResponse;
    }

    for (size_t i = 0; i < queueSize; i++) {
        Node *node              = _Queue.get(req.queue, i);
        char *nodeMessage       = _Node.getMessage(node);
        cJSON *nodeJSONObject   = cJSON_CreateObject();
        cJSON *message          = NULL;

        if (cJSON_AddNumberToObject(nodeJSONObject, "index", i) == NULL) {
            code = 500;
            errMsg = "Failed to add index to node JSON object.";
            goto createResponse;
        }

        if(cJSON_AddStringToObject(nodeJSONObject, "message", nodeMessage) == NULL) {
            code = 500;
            errMsg = "Failed to add message to node JSON object.";
            goto createResponse;
        }

        cJSON_AddItemToArray(nodesArray, nodeJSONObject);
    }

    nodesString = cJSON_Print(nodes);
    if (nodesString == NULL) {
        code = 500;
        errMsg = "Failed to print nodes.";
        goto createResponse;
    }

    data = (char *) mem_alloc(strlen(nodesString) + 1);
    code = 200;
    success = true;
    strcpy(data, nodesString);
    
    createResponse:
    res = create_response(success, code, msg, data, errMsg);

    mem_free(data);

    return res;
}

static struct broker_response get_all_queue_names(Broker *this, struct broker_request req)
{
    struct broker_response res;
    unsigned short int code;
    char *errMsg                    = NULL;
    bool success                    = false;
    char *data                      = NULL;
    const char **queueNamesArray    = queue_pool_get_all_names(this->queuePool);
    cJSON *queueNames               = cJSON_CreateObject();
    cJSON *nodesArray               = NULL;
    char *nodesString               = NULL;
    
    nodesArray = cJSON_AddArrayToObject(queueNames, "queueNames");
    if (nodesArray == NULL) {
        code = 500;
        errMsg = "Failed to add an array to JSON node list.";
        goto createResponse;
    }

    for (size_t i = 0; i < this->queuePool->length; i++) {
        cJSON *nodeJSONObject   = cJSON_CreateObject();
        cJSON *message          = NULL;

        if (cJSON_AddNumberToObject(nodeJSONObject, "index", i) == NULL) {
            code = 500;
            errMsg = "Failed to add index to node JSON object.";
            goto createResponse;
        }

        if(cJSON_AddStringToObject(nodeJSONObject, "name", queueNamesArray[i]) == NULL) {
            code = 500;
            errMsg = "Failed to add queue name to node JSON object.";
            goto createResponse;
        }

        cJSON_AddItemToArray(nodesArray, nodeJSONObject);
    }

    nodesString = cJSON_Print(queueNames);
    if (nodesString == NULL) {
        code = 500;
        errMsg = "Failed to print queue names.";
        goto createResponse;
    }

    data = (char *) mem_alloc(strlen(nodesString) + 1);
    code = 200;
    success = true;
    strcpy(data, nodesString);
    
    createResponse:
    res = create_response(success, code, NULL, data, errMsg);

    mem_free(data);
    mem_free(queueNamesArray);

    return res;
}

static struct broker_response length(Broker *this, struct broker_request req)
{
    struct broker_response  res;
    unsigned short int code;
    char *data              = NULL;
    bool success            = true;
    const int queueSize     = _Queue.size(req.queue);
    char *queueSize_str     = int_to_string(queueSize);
    char *errMsg            = NULL;

    data = (char *) mem_alloc(strlen(queueSize_str) + 1);
    code = 200;

    strcpy(data, queueSize_str);
    mem_free(queueSize_str);

    res = create_response(success, code, NULL, data, errMsg);
    mem_free(data);
    
    return res;
}

static struct broker_response produce(Broker *this, struct broker_request req)
{
    struct persist_request persist_req = {PERSIST_GET_NEXT_ID, req.queue->name, 0, NULL};
    struct persist_response persist_res = persist_dispatch(persist_req);
    
    struct broker_response res;
    unsigned short int code;
    bool success            = true;
    unsigned int newNodeId  = *((unsigned int *) persist_res.data);
    Node *newNode           = Node_new(newNodeId);
    char *msg               = NULL;
    char *errMsg            = NULL;

    mem_free(persist_res.data);

    _Node.setMessage(newNode, req.data, strlen(req.data));
    _Queue.add(req.queue, newNode);

    msg = "Message produced.";
    code = 200;
    
    res = create_response(success, code, msg, NULL, errMsg);

    return res;
}

static struct broker_response remove_queue(Broker *this, struct broker_request req)
{
    struct broker_response res;
    unsigned short int code;
    bool success        = false;
    char *msg           = NULL;
    char *errMsg        = NULL;

    Queue *queue = queue_pool_get_by_name(this->queuePool, req.queue->name);
    if (queue == NULL) {
        msg = "Failed: Queue with the given name doesn't exist.";
        goto createResponse;
    }

    if (queue_pool_remove_by_name(this->queuePool, req.queue->name)) {
        success = true;
        msg = "Queue removed.";
    }
    else {
        msg = "Failed to remove queue.";
    }

    createResponse:
    code = 200;
    
    res = create_response(success, code, msg, NULL, errMsg);

    return res;
}
