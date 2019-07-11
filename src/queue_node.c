#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "queue_node.h"

void Node_construct(Node *this)
{
    printf("construct\n");
}

Node *Node_new()
{
    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->mth = &_Node;
    newNode->mth->construct(newNode);
    return newNode;    
}

void Node_clear_message(Node *this) 
{
    if (this->message != NULL)
        free(this->message);
}

char *Node_get_message(Node *this) 
{
    return this->message;
}

void Node_set_message(Node *this, char *ms, int msSize)
{
    this->mth->clearMessage(this);
    this->message = (char *) malloc(sizeof(char) * msSize);
    strcpy(this->message, ms);
}

Node *Node_get_next(Node *this) 
{
    return this->next;
}

void Node_set_next(Node *this, Node *nxt)
{
    this->next = nxt;
}

void Node_destruct(Node *this)
{
    this->mth->clearMessage(this);
    free(this);
}