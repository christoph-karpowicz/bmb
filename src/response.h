#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "queue_node.h"
#include "request.h"

#define PROTOCOL "HTTP/1.1"
#define STATUS_200 "200 OK"
#define STATUS_204 "204 No Content"
#define STATUS_500 "500 Internal Server Error"
#define HEADER_ALLOW_ORIGIN "Access-Control-Allow-Origin: *"

typedef struct response 
{

    char *status;
    char **headers;
    unsigned short int headers_count;
    char *body;
    Request *req;
    Queue *queue;
    char *res_string;

    void (*construct)(void *this, Request *req, Queue *queue);
    void (*assemble)(const void *this);
    char *(*get)(const void *this);
    void (*handle)(const void *this);
    void (*destruct)(void *this);
    
} Response;

Response *Response_new(Request *req, Queue *queue);

static void Response_construct(void *this, Request *req, Queue *queue);

static void Response_assemble(const void *this);

static char *Response_get(const void *this);

static void Response_handle(const void *this);

static void Response_destruct(void *this);

#endif