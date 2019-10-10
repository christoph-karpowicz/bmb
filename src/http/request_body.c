#include "request_body.h"

RequestBody *RequestBody_new()
{

    RequestBody *newRequestBody = (RequestBody *) malloc(sizeof(RequestBody));

    // Assign properties.
    newRequestBody->pairs = (RequestPair **) malloc(0);
    
    // Assign methods.
    newRequestBody->add = &RequestBody_add;
    newRequestBody->get = &RequestBody_get;
    newRequestBody->destruct = &RequestBody_destruct;

    printf("Request body created... ");

    return newRequestBody;

}

void RequestBody_add(void *this, RequestPair *pair)
{

    RequestBody *self = (RequestBody *)this;
    const size_t pairs_length = sizeof(self->pairs) / sizeof(RequestPair *);
    self->pairs = (RequestPair **) realloc(self->pairs, sizeof(self->pairs) + sizeof(RequestPair *));
    self->pairs[pairs_length] = pair;
    printf("%s added to request body... ", pair->getKey(pair));

}

const char *RequestBody_get(void *this, const char *key)
{

    RequestBody *self = (RequestBody *)this;
    const size_t pairs_length = sizeof(self->pairs) / sizeof(RequestPair *);
    const char *empty = "";

    for (unsigned short int i = 0; i < pairs_length; i++)
    {
        RequestPair *pair = self->pairs[i];
        if (pair != NULL && strcmp(pair->getKey(pair), key) == 0) 
            return pair->getValue(pair);
    }

    return empty;
    
}

void RequestBody_destruct(void *this)
{

    RequestBody *self = (RequestBody *)this;
    const size_t pairs_length = sizeof(self->pairs) / sizeof(RequestPair *);

    for (unsigned short int i = 0; i < pairs_length; i++)
    {
        free(self->pairs[i]);
    }
    free(self->pairs);

}