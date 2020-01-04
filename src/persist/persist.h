#ifndef PERSIST_H
#define PERSIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../util/mem.h"

#define PERSIST_ADD_QUEUE       1
#define PERSIST_REMOVE_QUEUE    2
#define PERSIST_READ_QUEUE      3
#define PERSIST_ADD_NODE        4
#define PERSIST_REMOVE_NODE     5
#define PERSIST_READ_NODE       6

typedef struct persist 
{

} Persist;

struct persist_request {
    const unsigned short int    type;
    const char                  *queueName;
    const unsigned int          nodeIndex;
};

struct persist_response {
    char *errorMessage;
    bool success;
    void *data;
};

Persist *persist_init();

struct persist_response persist_dispatch();

static bool create_node_file(const char *path, const char *name, const char *content);

static bool create_queue_dir(const char *dirName);

static char *read_node(const char *path, const char *name);

static int *read_queue(const char *path);

static size_t get_queue_length(const char *path);

void persist_destruct(Persist *this);

#endif