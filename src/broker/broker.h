#ifndef BROKER_H
#define BROKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cjson/cJSON.h>

#include "../queue/queue.h"
#include "../queue/queue_node.h"
#include "../util/util.h"

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

static struct broker_response consume(Broker *this);

static struct broker_response create_response(bool success, char *message, char *data);

static struct broker_response get(Broker *this, struct broker_request req);

static struct broker_response length(Broker *this, struct broker_request req);

static struct broker_response produce(Broker *this, struct broker_request req);

#endif