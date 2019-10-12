#include "request_body.h"

RequestBody *RequestBody_new()
{

    RequestBody *newRequestBody = (RequestBody *) malloc(sizeof(RequestBody));

    // Assign properties.
    newRequestBody->pairs = (RequestPair **) malloc(0);
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
    self->pairs = (RequestPair **) realloc(self->pairs, self->pairs_length * sizeof(RequestPair *));
    self->pairs[self->pairs_length - 1] = pair;
    printf("%s added to request body... ", pair->getKey(pair));

}

const char *RequestBody_get(void *this, const char *key)
{

    RequestBody *self = (RequestBody *)this;
    const char *empty = "";

    for (unsigned short int i = 0; i < self->pairs_length; i++)
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

    for (unsigned short int i = 0; i < self->pairs_length; i++)
    {
        if (self->pairs[i] != NULL)
        {
            // printf("free pair %s\n", self->pairs[i]->getKey(self->pairs[i]));
            free(self->pairs[i]);
        }
    }
    free(self->pairs);
    free(this);

}