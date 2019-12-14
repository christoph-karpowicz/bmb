#include "request_body.h"

RequestBody *RequestBody_new()
{

    RequestBody *newRequestBody = (RequestBody *) mem_alloc(sizeof(RequestBody));

    // Assign properties.
    newRequestBody->pairs = NULL;
    newRequestBody->pairs_length = 0;
    
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
    self->pairs_length++;
    size_t pairsSize = self->pairs_length * sizeof(RequestPair *);
    if (self->pairs == NULL) {
        self->pairs = (RequestPair **) mem_alloc(pairsSize);
    }
    else {
        self->pairs = (RequestPair **) realloc(self->pairs, pairsSize);
    }
    self->pairs[self->pairs_length - 1] = pair;
    printf("%s added to request body... ", pair->getKey(pair));

}

const char *RequestBody_get(void *this, const char *key)
{

    RequestBody *self = (RequestBody *)this;

    for (unsigned short int i = 0; i < self->pairs_length; i++)
    {
        RequestPair *pair = self->pairs[i];
        if (pair != NULL && strcmp(pair->getKey(pair), key) == 0) 
            return pair->getValue(pair);
    }

    return NULL;
    
}

void RequestBody_destruct(void *this)
{

    RequestBody *self = (RequestBody *)this;

    for (unsigned short int i = 0; i < self->pairs_length; i++)
    {
        if (self->pairs[i] != NULL)
        {
            // printf("free pair %s\n", self->pairs[i]->getKey(self->pairs[i]));
            self->pairs[i]->destruct(self->pairs[i]);
        }
    }
    mem_free(self->pairs);
    mem_free(this);

}