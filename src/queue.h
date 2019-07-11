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

    struct queue_methods *mth;

    Node* root;
    
} Queue;

Queue *Queue_new();

void Queue_construct(Queue *this);

Node *Queue_get_root(Queue *this);

void Queue_set_root(Queue *this, Node *rt);

void Queue_destruct(Queue *this);

struct queue_methods
{

    void (*construct)(Queue *this);
    Node *(*getRoot)(Queue *this);
    void (*setRoot)(Queue *this, Node *rt);
    void (*destruct)(Queue *this);

} queue =
{
    .construct = Queue_construct,
    .getRoot = Queue_get_root,
    .setRoot = Queue_set_root,
    .destruct = Queue_destruct
};

#endif