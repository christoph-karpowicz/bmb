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

Queue *Queue_new()
{
    Queue *newQueue = (Queue *) malloc(sizeof(Queue));
    newQueue->mth = &_Queue;
    newQueue->mth->construct(newQueue);
    return newQueue;    
}

void Queue_construct(Queue *this)
{
    _Queue.reset(this);
    printf("Queue created.\n");
}

void Queue_add(Queue *this, Node *rt)
{

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

    printf("========================\n%s\n", _Node.getMessage(rt));
    
}

void Queue_clear(Queue *this)
{

    if (_Queue.isEmpty(this)) return;

    int queueSize = _Queue.size(this);
    
    Node **asArray = (Node **) malloc(sizeof(Node *) * queueSize);

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
        printf("clearing: %d: %s \n", j, _Node.getMessage(asArray[j]));
        _Node.destruct(asArray[j]);
    }

    free(asArray);

}

void Queue_display_all(const Queue *this)
{

    if (_Queue.isEmpty(this)) 
    {
        printf("----------------------\nQueue is empty.\n----------------------\n");
        return;
    }
    
    Node *el = this->root;
    int i = 1;
    do  
    {
        printf("----------------------\nMessage %d:\n%s\n\n", i, _Node.getMessage(el));
        el = _Node.getNext(el);
        i++;
    } while (el != NULL);

}

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

Node *Queue_get_next_tail(const Queue *this)
{

    int queueSize = _Queue.size(this);

    if (queueSize <= 1) return NULL;

    return _Queue.get(this, queueSize - 2);

}

Node *Queue_get_root(const Queue *this)
{
    return this->root;
}

bool Queue_is_empty(const Queue *this)
{
    return _Queue.size(this) > 0 ? false : true;
}

Node *Queue_peek(const Queue *this)
{
    return this->tail;
}

Node *Queue_poll(Queue *this)
{

    if (_Queue.isEmpty(this)) return NULL;

    Node *result = this->tail;
    Node *nextTail = _Queue.getNextTail(this);

    if (nextTail != NULL)
    {
        _Node.setNext(nextTail, NULL);
        this->tail = nextTail;
    }
    else 
        _Queue.reset(this);
    
    return result;

}

void Queue_reset(Queue *this)
{

    this->root = NULL;
    this->tail = NULL;

}

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

    free(this);

}