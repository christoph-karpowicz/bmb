#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include "queue_node.h"

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

void Queue_display_all(Queue *this);

Node *Queue_get(Queue *this, int index);

Node *Queue_get_root(Queue *this);

Node *Queue_get_next_tail(Queue *this);

bool Queue_is_empty(Queue *this);

Node *Queue_peek(Queue *this);

Node *Queue_poll(Queue *this);

int Queue_size(Queue *this);

void Queue_destruct(Queue *this);

struct queue_methods
{

    void (*construct)(Queue *this);
    void (*add)(Queue *this, Node *rt);
    void (*clear)(Queue *this);
    void (*displayAll)(Queue *this);
    Node *(*get)(Queue *this, int index);
    Node *(*getNextTail)(Queue *this);
    Node *(*getRoot)(Queue *this);
    bool (*isEmpty)(Queue *this);
    Node *(*peek)(Queue *this);
    Node *(*poll)(Queue *this);
    int (*size)(Queue *this);
    void (*destruct)(Queue *this);

}; 

extern const struct queue_methods _Queue;

#endif