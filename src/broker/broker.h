#ifndef BROKER_H
#define BROKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cjson/cJSON.h>

#include "../queue/queue.h"
#include "../queue/queue_node.h"

typedef struct {
    Queue *queue;
} Broker;

struct broker_request {
    const char          *type;
    unsigned short int  queue_id;
    unsigned short int  index;
    const char          *msg;
};

struct broker_response {
    cJSON *success;
    cJSON *payload;
};

Broker *broker_init();

struct broker_response broker_dispatch(Broker *this, struct broker_request broker_req);

struct broker_response consume(Broker *this);

struct broker_response produce(Broker *this, struct broker_request req);

#endif