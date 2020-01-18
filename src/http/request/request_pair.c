#include "request_pair.h"

/**
 * RequestPair_new - RequestPair struct constructor,
 * represents a single param => value pair extracted from an http request.
 * @key: param name
 * @value: param value
 * 
 * RETURNS:
 * new RequestPair struct
 */
RequestPair *RequestPair_new(const char *key, char *value)
{

    RequestPair *newRequestPair = (RequestPair *) mem_alloc(sizeof(RequestPair));

    // Assign properties.
    newRequestPair->key = key;
    newRequestPair->value = value;
    
    // Assign methods.
    newRequestPair->getKey = &RequestPair_getKey;
    newRequestPair->getValue = &RequestPair_getValue;
    newRequestPair->destruct = &RequestPair_destruct;

    return newRequestPair;

}

static const char *RequestPair_getKey(void *this)
{
    return ((RequestPair *)this)->key;
}

static char *RequestPair_getValue(void *this)
{
    return ((RequestPair *)this)->value;
}

static void RequestPair_destruct(void *this)
{
    mem_free(((RequestPair *)this)->value);
    mem_free(this);
}