#ifndef BROKER_H
#define BROKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cjson/cJSON.h>

#include "../persist/persist.h"
#include "../queue/queue.h"
#include "../queue/queue_node.h"
#include "../queue/queue_pool.h"
#include "../util/util.h"

#define BROKER_CONSUME              "consume"
#define BROKER_PRODUCE              "produce"
#define BROKER_LENGTH               "length"
#define BROKER_GET                  "get"
#define BROKER_GET_ALL              "getAll"
#define BROKER_ADD_QUEUE            "addQueue"
#define BROKER_GET_ALL_QUEUE_NAMES  "getAllQueueNames"
#define BROKER_REMOVE_QUEUE         "removeQueue"

typedef struct {
    QueuePool *queuePool;
} Broker;

struct broker_request {
    const char          *type;
    Queue               *queue;
    const char          *queueName;
    unsigned short int  index;
    const char          *data;
};

struct broker_response {
    unsigned short int  code;
    char                *errorMessage;
    cJSON               *success;
    cJSON               *payload;
};

Broker *broker_init();

struct broker_response broker_dispatch(Broker *this, struct broker_request broker_req);

static struct broker_response add_queue(Broker *this, struct broker_request req);

static struct broker_response consume(Broker *this, struct broker_request req);

static struct broker_response create_response(bool success, unsigned short int code, char *message, char *data, char *errorMessage);

static struct broker_response get(Broker *this, struct broker_request req);

static struct broker_response get_all(Broker *this, struct broker_request req);

static struct broker_response get_all_queue_names(Broker *this, struct broker_request req);

static struct broker_response length(Broker *this, struct broker_request req);

static struct broker_response produce(Broker *this, struct broker_request req);

static struct broker_response remove_queue(Broker *this, struct broker_request req);

#endif