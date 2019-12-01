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

    return res;
}

static struct broker_response consume(Broker *this)
{
    struct broker_response res;
    bool success = false;
    char *msg;
    char *data = NULL;

    if (!_Queue.isEmpty(this->queue)) {
        Node *node = _Queue.poll(this->queue);
        msg = NULL;
        data = (char *) mem_alloc(strlen(_Node.getMessage(node)) + 1);
        strcpy(data, _Node.getMessage(node));
        _Node.destruct(node);
        success = true;
    }
    else {
        msg = "Queue is empty.";
    }

    res = create_response(success, msg, data);
    mem_free(data);

    return res;
}

static struct broker_response create_response(bool success, char *message, char *data)
{
    struct broker_response res;
    cJSON *json_success = NULL;
    cJSON *json_payload = NULL;
    cJSON *json_message = NULL;
    cJSON *json_data    = NULL;

    json_success = cJSON_CreateBool(success);
    if (json_success == NULL)
    {
        exit(1);
    }

    json_payload = cJSON_CreateObject();

    if (message != NULL) {
        json_message = cJSON_CreateString(message);
        if (json_message == NULL)
        {
            exit(1);
        }
        cJSON_AddItemToObject(json_payload, "message", json_message);
    }

    if (data != NULL) {
        json_data = cJSON_CreateString(data);
        if (json_data == NULL)
        {
            exit(1);
        }
        cJSON_AddItemToObject(json_payload, "data", json_data);
    }

    res.success = json_success;
    res.payload = json_payload;

    printf("data: %s\n", data);

    return res;
}

static struct broker_response get(Broker *this, struct broker_request req)
{
    struct broker_response res;
    bool success = false;
    char *msg;
    char *data = NULL;

    const int queueSize =   _Queue.size(this->queue);
    if (queueSize > req.index) {
        Node *node = _Queue.get(this->queue, req.index);
        msg = NULL;
        data = (char *) mem_alloc(strlen(_Node.getMessage(node)) + 1);
        strcpy(data, _Node.getMessage(node));
        success = true;
    }
    else {
        msg = "Node index out of range.";
    }

    res = create_response(success, msg, data);

    mem_free(data);

    return res;
}

static struct broker_response length(Broker *this, struct broker_request req)
{
    struct broker_response  res;
    char                    *data;
    bool success            = true;
    const int queueSize     = _Queue.size(this->queue);
    char *queueSize_str     = intToString(queueSize);

    data = (char *) mem_alloc(strlen(queueSize_str) + 1);

    strcpy(data, queueSize_str);
    mem_free(queueSize_str);

    res = create_response(success, NULL, data);
    mem_free(data);
    
    return res;
}

static struct broker_response produce(Broker *this, struct broker_request req)
{
    struct broker_response res;
    bool success        = true;
    Node *newNode       = Node_new();
    char *msg;
    _Node.setMessage(newNode, req.msg, strlen(req.msg));
    _Queue.add(this->queue, newNode);

    msg = "Message produced.";
    
    res = create_response(success, msg, NULL);

    return res;
}