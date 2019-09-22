#include "response.h"

Response *Response_new(Request *req, Queue *queue) 
{

    Response *newResponse = (Response *) malloc(sizeof(Response));

    // Assign methods.
    newResponse->construct = &Response_construct;
    newResponse->assemble = &Response_assemble;
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

static void Response_assemble(const void *this)
{

    Response *self = (Response *)this;
    unsigned short int headers_length = 0;
    for (unsigned short int i = 0; i < self->headers_count; i++) {
        headers_length += (strlen(self->headers[i]) + 1);
    }

    printf("\nh-len: %d\n", headers_length);
    const size_t total_size = strlen(self->status) + 2 + headers_length + strlen(self->body) + 1;
    self->res_string = (char *) malloc(sizeof(char) * total_size);
    
    strcpy(self->res_string, self->status);
    strcat(self->res_string, "\n");
    for (unsigned short int i = 0; i < self->headers_count; i++) {
        strcat(self->res_string, self->headers[i]);
        strcat(self->res_string, "\n");
    }
    strcat(self->res_string, "\n");
    strcat(self->res_string, self->body);
    
    // printf("\n%s\n", self->res_string);
    
}

static char *Response_get(const void *this)
{
    return ((Response *)this)->res_string; 
}

static void Response_handle(const void *this)
{

    Response *self = (Response *)this;
    const Request *req = self->req;
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

        self->status = (char *) malloc(sizeof(char) * (strlen(PROTOCOL) + strlen(STATUS_200) + 2));
        strcpy(self->status, PROTOCOL);
        strcat(self->status, " ");
        strcat(self->status, STATUS_200);

        self->headers_count = 1;
        self->headers = (char **) malloc(sizeof(char *) * self->headers_count);
        self->headers[0] = (char *) malloc(sizeof(char) * (strlen(HEADER_ALLOW_ORIGIN) + 1));
        strcpy(self->headers[0], HEADER_ALLOW_ORIGIN);

        // printf("\n%s\n", self->status);

        const char *body = "Message produced.";
        self->body = (char *) malloc(sizeof(char) * (strlen(body) + 1));
        strcpy(self->body, body);
        // printf("\n%s\n", self->body);
        
    }
    
}

static void Response_destruct(void *this)
{
    free(((Response *)this)->status);
    for (unsigned short int i = 0; i < ((Response *)this)->headers_count; i++) {
        free(((Response *)this)->headers[i]);
    }
    free(((Response *)this)->headers);
    free(((Response *)this)->body);
    free(((Response *)this)->res_string);
    free(this);
}