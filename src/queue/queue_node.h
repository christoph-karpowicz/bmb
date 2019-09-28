#ifndef QUEUE_NODE_H
#define QUEUE_NODE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct node Node; 

struct node 
{
    
    const struct node_methods *mth;

    char *message;
    Node *next;
    
};

Node *Node_new();

void Node_construct(Node *this);

void Node_clear_message(Node *this);

char *Node_get_message(const Node *this);

void Node_set_message(Node *this, char *ms, int msSize);

Node *Node_get_next(const Node *this);

void Node_set_next(Node *this, Node *nxt);

void Node_destruct(Node *this);

struct node_methods 
{

    void (*construct)(Node *this);
    void (*clearMessage)(Node *this);
    char *(*getMessage)(const Node *this);
    void (*setMessage)(Node *this, char *ms, int msSize);
    Node *(*getNext)(const Node *this);
    void (*setNext)(Node *this, Node *nxt);
    void (*destruct)(Node *this);
    
}; 

extern const struct node_methods _Node;

#endif