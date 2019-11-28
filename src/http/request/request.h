#ifndef REQUEST_H
#define REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>

#include "request_body.h"
#include "request_pair.h"
#include "../../util/mem.h"

typedef struct request 
{

    char *method;
    RequestBody *body;
    
} Request;

Request *Request_parse(const char *req);

static RequestPair *Request_extract_data(const char *request, const char *key);

static char *Request_extract_method(const char *request);

void Request_destruct(Request *this);

#endif