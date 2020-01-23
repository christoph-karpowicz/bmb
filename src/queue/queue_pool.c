#include "queue_pool.h"

/**
 * queue_pool_init - QueuePool struct constructor.
 * 
 * This struct stores pointers to all queues in the app. 
 * 
 * RETURNS:
 * new QueuePool struct
 */
QueuePool *queue_pool_init() 
{
    QueuePool *newQueuePool = (QueuePool *) mem_alloc(sizeof(QueuePool));
    newQueuePool->length    = 0;
    newQueuePool->pool      = NULL;
    return newQueuePool;
}

/**
 * queue_pool_add - adds a new queue to the pool.
 * @this: QueuePool instance
 * @name: new queue's name
 * @init: is this function called on app startup
 * 
 * RETURNS:
 * was the action successful
 */
bool queue_pool_add(QueuePool *this, const char *name, const bool init)
{
    Queue *newQueue = Queue_new(name);

    if (!init) {
        struct persist_request preq = {PERSIST_ADD_QUEUE, name, 0, NULL};
        struct persist_response pres = persist_dispatch(preq);
        if (!pres.success) {
            fprintf(stderr, "%s\n", pres.errorMessage);
            return false;
        }
    }
    
    if (this->length == 0) {
        this->pool = (Queue **) mem_alloc(sizeof(Queue *));
    }
    else {
        this->pool = (Queue **) realloc(this->pool, sizeof(Queue *) * (this->length + 1));
    }

    if (this->pool == NULL) 
        return false;

    this->pool[this->length] = newQueue;
    this->length++;
    
    return true;
}

/**
 * queue_pool_clear_nulls - removes NULL pointers from the pool.
 * @this: QueuePool instance
 * 
 * Gets called whenever a queue has been removed from the pool.
 * Shrinks the pool array.
 */
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

    this->length = this->length - nullCount;
    if (this->length < 0) exit(1);

    if (this->length == 0)
        mem_free(this->pool);
    else
        this->pool = (Queue **) realloc(this->pool, sizeof(Queue*) * this->length);

    for (size_t i = 0; i < this->length; i++) {
        this->pool[i] = tmpPool[i];
    }
}

/**
 * queue_pool_get_all_names
 * @this: QueuePool instance
 * 
 * RETURNS:
 * a dynamically allocated array of all queue names in the pool
 */
const char **queue_pool_get_all_names(QueuePool *this)
{
    const char **queueNames = (const char **) mem_alloc(sizeof(const char *) * this->length);
    for (size_t i = 0; i < this->length; i++) {
        queueNames[i] = this->pool[i]->name;
    }
    return queueNames;
}

/**
 * queue_pool_get_by_name - gets a queue with a given name.
 * @this: QueuePool instance
 * @name: queue name
 * 
 * RETURNS:
 * found queue pointer
 */
Queue *queue_pool_get_by_name(QueuePool *this, const char *name)
{
    if (name == NULL) 
        return NULL;
    
    for (size_t i = 0; i < this->length; i++) {
        if (strcmp(this->pool[i]->name, name) == 0) 
            return this->pool[i];
    }

    return NULL;
}

/**
 * queue_pool_load - loads the queue pool from the file system.
 * @this: QueuePool instance
 * 
 * Called on app startup, uses the persistence API 
 * to read all the queues into memory.
 */
void queue_pool_load(QueuePool *this)
{
    log("Loading queue pool...\n");
    struct persist_request preq = {PERSIST_GET_QUEUE_LIST, NULL, 0, NULL};
    struct persist_response pres = persist_dispatch(preq);
    if (!pres.success) {
        fprintf(stderr, "%s\n", pres.errorMessage);
        exit(1);
    }

    void **queueList = pres.data;
    size_t numberOfQueues = *((size_t *) queueList[0]);
    log("Number of queues found: %ld\n", numberOfQueues);
    // Create queues.
    for (size_t i = 1; i <= numberOfQueues; i++) {
        char *queueName = (char *) queueList[i];
        struct persist_request pqreq = {PERSIST_READ_QUEUE, queueName, 0, NULL};
        struct persist_response pqres = persist_dispatch(pqreq);

        if (!pqres.success) {
            fprintf(stderr, "%s\n", pqres.errorMessage);
            exit(1);
        }

        int *queueNodes = pqres.data;

        queue_pool_add(this, queueName, true);
        Queue *queue = queue_pool_get_by_name(this, queueName);
        queue->persist = false;

        // Create nodes.
        log("Number of nodes found: %d\n", queueNodes[0]);
        for (size_t i = 1; i <= queueNodes[0]; i++) {
            struct persist_request pnreq = {PERSIST_READ_NODE, queueName, queueNodes[i], NULL};
            struct persist_response pnres = persist_dispatch(pnreq);

            if (!pnres.success) {
                fprintf(stderr, "%s\n", pnres.errorMessage);
                exit(1);
            }

            char *nodeContent = pnres.data;
            Node *newNode = Node_new(queueNodes[i]);
            _Node.setMessage(newNode, nodeContent, strlen(nodeContent));
            _Queue.add(queue, newNode);

            mem_free(nodeContent);
            log("Loaded node: %d\n", queueNodes[i]);
        }

        // Get and show next node id in currently loaded queue.
        struct persist_request pidreq = {PERSIST_GET_NEXT_ID, queueName, 0, NULL};
        struct persist_response pidres = persist_dispatch(pidreq);
        log("Next id will be: %d\n", *((unsigned int *) pidres.data));
        mem_free(pidres.data);

        mem_free(queueNodes);
        queue->persist = true;
        log("====\n");

    }

    for (size_t i = 0; i <= numberOfQueues; i++) {
        mem_free(queueList[i]);
    }
    mem_free(queueList);
    log("Queue pool loaded.\n\n");
}

/**
 * queue_pool_remove_by_name - remove a queue with a given name from the pool.
 * @this: QueuePool instance
 * 
 * RETURNS:
 * was the action successful
 */
bool queue_pool_remove_by_name(QueuePool *this, const char *name)
{
    struct persist_request preq = {PERSIST_REMOVE_QUEUE, name, 0, NULL};
    struct persist_response pres = persist_dispatch(preq);
    if (!pres.success) {
        fprintf(stderr, "%s\n", pres.errorMessage);
        mem_free(pres.errorMessage);
        return false;
    }

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
    mem_free(this);
}
