#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

#include "queue_node.h"
#include "../util/mem.h"

typedef struct queue 
{

    const struct queue_methods *mth;

    Node* root;
    Node* tail;
    
} Queue;

Queue *Queue_new();

void Queue_construct(Queue *this);

void Queue_add(Queue *this, Node *rt);

void Queue_clear(Queue *this);

void Queue_display_all(const Queue *this);

Node *Queue_get(const Queue *this, const int index);

Node *Queue_get_root(const Queue *this);

Node *Queue_get_next_tail(const Queue *this);

bool Queue_is_empty(const Queue *this);

Node *Queue_peek(const Queue *this);

Node *Queue_poll(Queue *this);

void Queue_reset(Queue *this);

int Queue_size(const Queue *this);

void Queue_destruct(Queue *this);

struct queue_methods
{

    void (*construct)(Queue *this);
    void (*add)(Queue *this, Node *rt);
    void (*clear)(Queue *this);
    void (*displayAll)(const Queue *this);
    Node *(*get)(const Queue *this, const int index);
    Node *(*getNextTail)(const Queue *this);
    Node *(*getRoot)(const Queue *this);
    bool (*isEmpty)(const Queue *this);
    Node *(*peek)(const Queue *this);
    Node *(*poll)(Queue *this);
    void (*reset)(Queue *this);
    int (*size)(const Queue *this);
    void (*destruct)(Queue *this);

}; 

extern const struct queue_methods _Queue;

#endif