#include "queue_node.h"

const struct node_methods _Node = 
{

    .construct = Node_construct,
    .clearMessage = Node_clear_message,
    .getMessage = Node_get_message,
    .setMessage = Node_set_message,
    .getNext = Node_get_next,
    .setNext = Node_set_next,
    .destruct = Node_destruct
    
};

void Node_construct(Node *this)
{
    this->message = NULL;
    this->next = NULL;
}

Node *Node_new(unsigned int id)
{
    Node *newNode = (Node *) mem_alloc(sizeof(Node));
    newNode->id = id;
    newNode->mth = &_Node;
    newNode->mth->construct(newNode);
    return newNode;
}

void Node_clear_message(Node *this) 
{
    if (this->message)
        mem_free(this->message);
}

char *Node_get_message(const Node *this) 
{
    return this->message;
}

void Node_set_message(Node *this, const char *ms, int msSize)
{ 
    this->mth->clearMessage(this);
    this->message = (char *) mem_alloc(sizeof(char) * (msSize + 1));
    strcpy(this->message, ms);
}

Node *Node_get_next(const Node *this) 
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
    mem_free(this);
}