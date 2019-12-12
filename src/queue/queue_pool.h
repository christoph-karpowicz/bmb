#ifndef QUEUE_POOL_H
#define QUEUE_POOL_H

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "../util/mem.h"

typedef struct queue_pool 
{

    size_t  length;
    Queue   **pool;
    
} QueuePool;

QueuePool *queue_pool_init();

bool queue_pool_add(QueuePool *this, const char *name);

static void queue_pool_clear_nulls(QueuePool *this);

const char **queue_pool_get_all_names(QueuePool *this);

Queue *queue_pool_get_by_name(QueuePool *this, const char *name);

bool queue_pool_remove_by_name(QueuePool *this, const char *name);

QueuePool *queue_pool_destruct(QueuePool *this);

#endif