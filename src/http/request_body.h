#ifndef REQUEST_BODY_H
#define REQUEST_BODY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "request_pair.h"
#include "../util/mem.h"

typedef struct request_body 
{

    RequestPair **pairs;
    size_t pairs_length;

    const char *(*get)(void *this, const char *key);
    void (*add)(void *this, RequestPair *pair);
    void (*destruct)(void *this);
    
} RequestBody;

RequestBody *RequestBody_new();

void RequestBody_add(void *this, RequestPair *pair);

const char *RequestBody_get(void *this, const char *key);

void RequestBody_destruct(void *this);

#endif