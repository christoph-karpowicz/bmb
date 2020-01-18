#include "request_body.h"

/**
 * RequestBody_new - RequestBody struct constructor.
 * This struct stores param=>value pairs extracted from an http request.
 * 
 * RETURNS:
 * new RequestBody struct
 */
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

/**
 * RequestBody_add - adds a new RequestPair to the struct.
 * @this: RequestBody to which a pair i being added
 * @pair: added pair
 */
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

/**
 * RequestBody_get - finds a pair by param name.
 * @this: RequestBody to which a pair i being added
 * @key: param name
 * 
 * RETURNS:
 * param value
 */
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
            self->pairs[i]->destruct(self->pairs[i]);
        }
    }
    mem_free(self->pairs);
    mem_free(this);

}