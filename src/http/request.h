#ifndef REQUEST_H
#define REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "request_pair.h"

typedef struct request 
{

    char *method;
    char *message;
    
} Request;

Request *Request_parse(const char *req);

RequestPair *Request_extract_data(const char *request, const char *key);

char **Request_extract_method(const char *request);

void Request_destruct(Request *this);

#endif