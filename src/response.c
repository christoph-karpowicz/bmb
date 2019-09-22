#include "response.h"

#define PROTOCOL "HTTP/1.1"
#define RESPONSE_200 "200 OK"
#define RESPONSE_204 "204 No Content"
#define RESPONSE_500 "500 Internal Server Error"

Response *Response_new(Request *req, Queue *queue) 
{

    Response *newResponse = (Response *) malloc(sizeof(Response));

    // Assign methods.
    newResponse->construct = &Response_construct;
    newResponse->get = &Response_get;
    newResponse->handle = &Response_handle;
    newResponse->destruct = &Response_destruct;
    
    newResponse->construct(newResponse, req, queue);
    return newResponse;    

}

static void Response_construct(void *this, Request *req, Queue *queue) 
{

    ((Response *)this)->req = req;
    ((Response *)this)->queue = queue;

}

static char *Response_get(const void *this)
{

    Response *self = (Response *)this;
    self->res_string = (char *) malloc(sizeof(char) * (
        strlen(self->status) + 2 +
        strlen(self->body)
    ));
    strcpy(self->res_string, self->status);
    strcat(self->res_string, "\n");
    strcat(self->res_string, "\n");
    strcat(self->res_string, self->body);
    
    return self->res_string; 
}

static void Response_handle(const void *this)
{

    Response *self = (Response *)this;
    Request *req = self->req;
    Queue *queue = self->queue;

    // Consume.
    if (strcmp(req->method, "GET") == 0)
    {



    }
    // Produce.
    else if (strcmp(req->method, "POST") == 0)
    {

        Node *newNode = Node_new();
        _Node.setMessage(newNode, req->message, strlen(req->message));
        _Queue.add(queue, newNode);

        self->status = (char *) malloc(sizeof(char) * (strlen(PROTOCOL) + strlen(RESPONSE_200) + 1));
        strcpy(self->status, PROTOCOL);
        strcat(self->status, " ");
        strcat(self->status, RESPONSE_200);

        printf("\n%s\n", self->status);

        char *body = "Message produced.";
        self->body = (char *) malloc(sizeof(char) * strlen(body));
        strcpy(self->body, body);
        printf("\n%s\n", self->body);
        
    }
    
}

static void Response_destruct(void *this)
{
    free(((Response *)this)->status);
    // free(((Response *)this)->headers);
    free(((Response *)this)->body);
    free(this);
}