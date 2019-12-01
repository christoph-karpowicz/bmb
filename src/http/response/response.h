#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <cjson/cJSON.h>

#include "../../queue/queue.h"
#include "../../queue/queue_node.h"
#include "../../broker/broker.h"
#include "../request/request.h"
#include "../../util/util.h"
#include "../../util/mem.h"

#define PROTOCOL "HTTP/1.1"
#define STATUS_200 "200 OK"
#define STATUS_204 "204 No Content"
#define STATUS_400 "400 Bad request"
#define STATUS_500 "500 Internal Server Error"
#define HEADER_ALLOW_ORIGIN "Access-Control-Allow-Origin: *"
#define HEADER_CONNECTION_CLOSE "Connection: close"
#define HEADER_CONTENT_TYPE "Content-type: text/html; charset=UTF-8"
#define HEADER_HOST "Host: localhost:8080"

typedef struct response 
{

    char *status;
    char **headers;
    unsigned short int headers_count;
    cJSON *json_body;
    char *body;
    Request *req;
    Broker *broker;
    bool success;
    char *error;
    char *res_string;

    void (*construct)(void *this, Request *req, Broker *broker);
    void (*assemble)(const void *this);
    char *(*get)(const void *this);
    void (*handle)(const void *this);
    struct broker_response (*handleGET)(const void *this, char **msg);
    struct broker_response (*handlePOST)(const void *this, char **msg);
    void (*setError)(const void *this, const char *msg);
    void (*setHeaders)(const void *this);
    void (*setStatus)(const void *this, unsigned short int code);
    void (*setSuccess)(const void *this, bool success);
    void (*destruct)(void *this);
    
} Response;

Response *Response_new(Request *req, Broker *broker);

static void Response_construct(void *this, Request *req, Broker *broker);

static void Response_assemble(const void *this);

static char *Response_get(const void *this);

static void Response_handle(const void *this);

static struct broker_response Response_handle_GET(const void *this, char **msg);

static struct broker_response Response_handle_POST(const void *this, char **msg);

static void Response_set_error(const void *this, const char *msg);

static void Response_set_headers(const void *this);

static void Response_set_status(const void *this, unsigned short int code);

static void Response_set_success(const void *this, bool success);

static void Response_destruct(void *this);

#endif