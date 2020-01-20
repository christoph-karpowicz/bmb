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

// @todo: to be removed
void Node_construct(Node *this)
{
    this->message = NULL;
    this->next = NULL;
}

/**
 * Node_new - Node struct constructor.
 * @id: node id
 * 
 * Nodes are stored by queues. 
 * They are given an unsigned int id 
 * and a string message.
 * Because each queue is a linked list, every node
 * stores a pointer to the next node in the queue.
 * 
 * RETURNS:
 * new Node struct
 */
Node *Node_new(unsigned int id)
{
    Node *newNode = (Node *) mem_alloc(sizeof(Node));
    newNode->id = id;
    newNode->mth = &_Node;
    newNode->mth->construct(newNode);
    return newNode;
}

/**
 * Node_clear_message
 * @this: Node instance
 */
void Node_clear_message(Node *this)
{
    if (this->message)
        mem_free(this->message);
}

/**
 * Node_get_message
 * @this: Node instance
 * 
 * RETURNS:
 * node's string message
 */
char *Node_get_message(const Node *this) 
{
    return this->message;
}

/**
 * Node_set_message
 * @this: Node instance
 * @ms: message
 * @msSize: message length
 */
void Node_set_message(Node *this, const char *ms, int msSize)
{ 
    this->mth->clearMessage(this);
    this->message = (char *) mem_alloc(sizeof(char) * (msSize + 1));
    strcpy(this->message, ms);
}

/**
 * Node_get_next
 * @this: Node instance
 * 
 * RETURNS:
 * pointer to the node occupying the next position in the queue
 */
Node *Node_get_next(const Node *this) 
{
    return this->next;
}

/**
 * Node_set_next - sets a pointer to the next node.
 * @this: Node instance
 */
void Node_set_next(Node *this, Node *nxt)
{
    this->next = nxt;
}

void Node_destruct(Node *this)
{
    this->mth->clearMessage(this);
    mem_free(this);
}