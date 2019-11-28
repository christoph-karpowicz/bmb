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

    return res;
}

struct broker_response consume(Broker *this)
{
    struct broker_response res;
    bool success = false;
    cJSON *json_success = NULL;
    cJSON *json_payload = NULL;
    char *msg;

    if (!_Queue.isEmpty(this->queue)) {
        Node *node = _Queue.poll(this->queue);
        msg = _Node.getMessage(node);
        _Node.destruct(node);
        success = true;
    }
    else {
        msg = "Queue is empty.";
    }

    json_success = cJSON_CreateBool(success);
    if (json_success == NULL)
    {
        exit(1);
    }

    json_payload = cJSON_CreateString(msg);
    if (json_payload == NULL)
    {
        exit(1);
    }

    res.success = json_success;
    res.payload = json_payload;

    return res;

}

struct broker_response produce(Broker *this, struct broker_request req)
{
    struct broker_response res;
    bool success        = true;
    cJSON *json_success = NULL;
    cJSON *json_payload = NULL;
    Node *newNode       = Node_new();
    char *msg;
    _Node.setMessage(newNode, req.msg, strlen(req.msg));
    _Queue.add(this->queue, newNode);

    msg = "Message produced.";
    
    json_success = cJSON_CreateBool(success);
    if (json_success == NULL)
    {
        exit(1);
    }

    json_payload = cJSON_CreateString(msg);
    if (json_payload == NULL)
    {
        exit(1);
    }

    res.success = json_success;
    res.payload = json_payload;

    return res;
}