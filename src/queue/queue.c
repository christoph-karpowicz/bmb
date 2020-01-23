#include "queue.h"

const struct queue_methods _Queue =
{
    .construct = Queue_construct,
    .add = Queue_add,
    .clear = Queue_clear,
    .displayAll = Queue_display_all,
    .get = Queue_get,
    .getNextTail = Queue_get_next_tail,
    .getRoot = Queue_get_root,
    .isEmpty = Queue_is_empty,
    .peek = Queue_peek,
    .poll = Queue_poll,
    .reset = Queue_reset,
    .size = Queue_size,
    .destruct = Queue_destruct
};

/**
 * Queue_new - Queue struct constructor.
 * @name: queue name
 * 
 * This struct is a linked list of Nodes. 
 * 
 * RETURNS:
 * new Queue struct
 */
Queue *Queue_new(const char *name)
{
    Queue *newQueue     = (Queue *) mem_alloc(sizeof(Queue));
    newQueue->name      = NULL;
    newQueue->persist   = true;
    newQueue->mth       = &_Queue;

    newQueue->mth->construct(newQueue, name);
    return newQueue;
}

// @todo: to be removed
void Queue_construct(Queue *this, const char *name)
{
    this->name = (char *) mem_alloc(strlen(name) + 1);
    strcpy(this->name, name);
    _Queue.reset(this);
    log("Queue \"%s\" created.\n", this->name);
}

/**
 * Queue_add - adds a new node to the queue.
 * @this: queue instance
 * @rt: new node
 * 
 * The node added to the queue becomes the new root.
 * 
 * RETURNS:
 * was the operation successful
 */
bool Queue_add(Queue *this, Node *rt)
{
    if (this->persist) {
        struct persist_request pidreq = {PERSIST_GET_NEXT_ID, this->name, 0, NULL};
        struct persist_response pidres = persist_dispatch(pidreq);
        unsigned int nextId = *((unsigned int *) pidres.data);
        mem_free(pidres.data);

        struct persist_request preq = {PERSIST_ADD_NODE, this->name, nextId, _Node.getMessage(rt)};
        struct persist_response pres = persist_dispatch(preq);
        if (!pres.success) {
            fprintf(stderr, "%s\n", pres.errorMessage);
            return false;
        }
    }

    if (this->root != NULL)
    {
        _Node.setNext(rt, this->root);
    }
    else
    {
        _Node.setNext(rt, NULL);
        this->tail = rt;
    }
    this->root = rt;

    // log("========================\n%s\n", _Node.getMessage(rt));
    return true;
}

/**
 * Queue_clear - removes all nodes from the queue.
 * @this: queue instance
 */
void Queue_clear(Queue *this)
{
    if (_Queue.isEmpty(this)) return;

    log("Clearing queue...\n");

    int queueSize = _Queue.size(this);
    
    Node *asArray[queueSize];

    Node *el = this->root;
    int i = 0;
    do  
    {
        asArray[i] = el;
        el = _Node.getNext(el);
        i++;
        
    } while (el != NULL);

    for (int j = 0; j < queueSize; j++)
    {
        log("clearing: %d: %s \n", j, _Node.getMessage(asArray[j]));
        _Node.destruct(asArray[j]);
    }

}

/**
 * Queue_display_all - displays a list of all the nodes in the terminal.
 * @this: queue instance
 */
void Queue_display_all(const Queue *this)
{

    if (_Queue.isEmpty(this)) 
    {
        log("----------------------\nQueue is empty.\n----------------------\n");
        return;
    }
    
    Node *el = this->root;
    int i = 1;
    do  
    {
        log("----------------------\nMessage %d:\n%s\n\n", i, _Node.getMessage(el));
        el = _Node.getNext(el);
        i++;
    } while (el != NULL);

}

/**
 * Queue_get - gets a node with the given index.
 * @this: queue instance
 * @index: node index
 * 
 * RETURNS:
 * found Node pointer
 */
Node *Queue_get(const Queue *this, const int index)
{

    Node *el;

    for (int i = 0; i <= index; i++) 
    {
        if (i == 0) el = this->root;
        else el = _Node.getNext(el);
    }

    return el;

}

/**
 * Queue_get_next_tail - finds a pointer to a node 
 * that's next to be the tail of the queue.
 * @this: queue instance
 * 
 * RETURNS:
 * new tail Node pointer
 */
Node *Queue_get_next_tail(const Queue *this)
{

    int queueSize = _Queue.size(this);

    if (queueSize <= 1) return NULL;

    return _Queue.get(this, queueSize - 2);

}

/**
 * Queue_get_root 
 * @this: queue instance
 * 
 * RETURNS:
 * root Node pointer
 */
Node *Queue_get_root(const Queue *this)
{
    return this->root;
}

/**
 * Queue_is_empty - check if the queue's empty.
 * @this: queue instance
 * 
 * RETURNS:
 * is the queue empty
 */
bool Queue_is_empty(const Queue *this)
{
    return _Queue.size(this) > 0 ? false : true;
}

/**
 * Queue_peek - gets the tail node's pointer without
 * removing it from the queue.
 * @this: queue instance
 * 
 * RETURNS:
 * tail Node pointer
 */
Node *Queue_peek(const Queue *this)
{
    return this->tail;
}

/**
 * Queue_poll - gets the tail node's pointer and
 * removes the node from the queue.
 * @this: queue instance
 * 
 * RETURNS:
 * tail Node pointer
 */
Node *Queue_poll(Queue *this)
{
    if (_Queue.isEmpty(this)) return NULL;

    Node *tail = this->tail;
    Node *nextTail = _Queue.getNextTail(this);

    if (this->persist) {
        struct persist_request preq = {PERSIST_REMOVE_NODE, this->name, tail->id, NULL};
        struct persist_response pres = persist_dispatch(preq);
        if (!pres.success) {
            fprintf(stderr, "%s\n", pres.errorMessage);
            return NULL;
        }
    }

    if (nextTail != NULL)
    {
        _Node.setNext(nextTail, NULL);
        this->tail = nextTail;
    }
    else 
        _Queue.reset(this);
    
    return tail;

}

/**
 * Queue_reset - resets root and tail pointer to NULL.
 * @this: queue instance
 */
void Queue_reset(Queue *this)
{
    this->root = NULL;
    this->tail = NULL;
}

/**
 * Queue_size - gets the queue's length.
 * @this: queue instance
 * 
 * RETURNS:
 * queue length
 */
int Queue_size(const Queue *this)
{

    if (this->root == NULL) return 0;

    int counter = 1;
    Node *element = this->root;
    while (_Node.getNext(element) != NULL)
    {
        element = _Node.getNext(element);
        counter++;
    }
    
    return counter;

}

void Queue_destruct(Queue *this)
{
    mem_free(this->name);
    mem_free(this);
}