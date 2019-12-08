#include "broker.h"

Broker *broker_init()
{
    Broker *broker = (Broker *) mem_alloc(sizeof(Broker));
    broker->queue = Queue_new();
    return broker;
}

struct broker_response broker_dispatch(Broker *this, struct broker_request broker_req)
{
    struct broker_response res;
    
    if (strcmp(broker_req.type, "consume") == 0) {
        res = consume(this);
    }
    else if (strcmp(broker_req.type, "produce") == 0) {
        res = produce(this, broker_req);
    }
    else if (strcmp(broker_req.type, "length") == 0) {
        res = length(this, broker_req);
    }
    else if (strcmp(broker_req.type, "get") == 0) {
        res = get(this, broker_req);
    }
    else if (strcmp(broker_req.type, "getAll") == 0) {
        res = getAll(this, broker_req);
    }

    return res;
}

static struct broker_response consume(Broker *this)
{
    struct broker_response res;
    unsigned short int code;
    bool success    = false;
    char *msg       = NULL;
    char *data      = NULL;
    char *errMsg    = NULL;

    if (!_Queue.isEmpty(this->queue)) {
        Node *node = _Queue.poll(this->queue);
        data = (char *) mem_alloc(strlen(_Node.getMessage(node)) + 1);
        strcpy(data, _Node.getMessage(node));
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

    printf("data: %s\n", data);

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

    const int queueSize =   _Queue.size(this->queue);
    if (queueSize > req.index) {
        Node *node = _Queue.get(this->queue, req.index);
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

static struct broker_response getAll(Broker *this, struct broker_request req)
{
    struct broker_response res;
    unsigned short int code;
    char *msg               = NULL;
    char *errMsg            = NULL;
    bool success            = false;
    char *data              = NULL;
    const int queueSize     = _Queue.size(this->queue);
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
        Node *node              = _Queue.get(this->queue, i);
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

static struct broker_response length(Broker *this, struct broker_request req)
{
    struct broker_response  res;
    unsigned short int code;
    char *data              = NULL;
    bool success            = true;
    const int queueSize     = _Queue.size(this->queue);
    char *queueSize_str     = intToString(queueSize);
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
    struct broker_response res;
    unsigned short int code;
    bool success        = true;
    Node *newNode       = Node_new();
    char *msg           = NULL;
    char *errMsg        = NULL;

    _Node.setMessage(newNode, req.msg, strlen(req.msg));
    _Queue.add(this->queue, newNode);

    msg = "Message produced.";
    code = 200;
    
    res = create_response(success, code, msg, NULL, errMsg);

    return res;
}