#ifndef REQUEST_PAIR_H
#define REQUEST_PAIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../util/mem.h"

typedef struct request_pair 
{

    const char *key;
    char *value;

    const char *(*getKey)(void *this);
    char *(*getValue)(void *this);
    void (*destruct)(void *this);
    
} RequestPair;

RequestPair *RequestPair_new(const char *key, char *value);

static const char *RequestPair_getKey(void *this);

static char *RequestPair_getValue(void *this);

static void RequestPair_destruct(void *this);

#endif