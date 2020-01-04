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

typedef struct persist 
{

} Persist;

struct persist_request {
    const unsigned short int type;
    const char *queueName;
    const unsigned int nodeIndex;
};

struct persist_response {
    char *errorMessage;
    bool success;
};

Persist *persist_init();

struct persist_response persist_dispatch();

static bool create_node_file(const char *path, const char *name, const char *content);

static bool create_queue_dir(const char *dirName);

void persist_destruct(Persist *this);

#endif