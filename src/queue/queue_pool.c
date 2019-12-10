#include "queue_pool.h"

QueuePool *queue_pool_init() 
{
    QueuePool *newQueuePool = (QueuePool *) mem_alloc(sizeof(QueuePool));
    newQueuePool->length    = 0;
    newQueuePool->pool      = NULL;
    return newQueuePool;
}

bool queue_pool_add(QueuePool *this, const char *name)
{
    Queue *newQueue = Queue_new(name);
    
    if (this->length == 0) {
        this->pool = (Queue **) mem_alloc(sizeof(Queue*));
    }
    else {
        this->pool = (Queue **) realloc(this->pool, sizeof(Queue*) * (this->length + 1));
    }

    if (this->pool == NULL) 
        return false;

    this->pool[this->length] = newQueue;
    this->length++;
    
    return true;
}

static void queue_pool_clear_nulls(QueuePool *this)
{
    size_t nullCount    = 0;
    size_t tmpPoolIndex = 0;
    Queue *tmpPool[this->length];
    
    for (size_t i = 0; i < this->length; i++) {
        if (this->pool[i] == NULL) nullCount++;
        else {
            tmpPool[tmpPoolIndex++] = this->pool[i];
        }
    }

    this->length = this->length - 2;
    if (this->length < 0) exit(1);

    if (this->length == 0) 
        this->pool = NULL;
    else
        this->pool = (Queue **) realloc(this->pool, sizeof(Queue*) * this->length);

    for (size_t i = 0; i < this->length; i++) {
        this->pool[i] = tmpPool[i];
    }
}

Queue *queue_pool_get_by_name(QueuePool *this, const char *name)
{
    for (size_t i = 0; i < this->length; i++) {
        if (strcmp(this->pool[i]->name, name) == 0) 
            return this->pool[i];
    }

    return NULL;
}

bool queue_pool_remove_by_name(QueuePool *this, const char *name)
{
    for (size_t i = 0; i < this->length; i++) {
        if (strcmp(this->pool[i]->name, name) == 0) {
            _Queue.clear(this->pool[i]);
            _Queue.destruct(this->pool[i]);
            this->pool[i] = NULL;
            queue_pool_clear_nulls(this);
            return true;
        }
    }

    return false;
}

QueuePool *queue_pool_destruct(QueuePool *this)
{
    for (size_t i = 0; i < this->length; i++) {
        _Queue.clear(this->pool[i]);
        _Queue.destruct(this->pool[i]);
    }
    mem_free(this->pool);
}
