#include "response.h"

Response *Response_new(Request *req, Queue *queue) 
{

    Response *newResponse = (Response *) malloc(sizeof(Response));

    // Assign methods.
    newResponse->construct = &Response_construct;
    newResponse->assemble = &Response_assemble;
    newResponse->get = &Response_get;
    newResponse->handle = &Response_handle;
    newResponse->setHeaders = &Response_set_headers;
    newResponse->setStatus = &Response_set_status;
    newResponse->destruct = &Response_destruct;

    newResponse->construct(newResponse, req, queue);
    return newResponse;

}

static void Response_construct(void *this, Request *req, Queue *queue) 
{

    printf("Response constructor called.\n");
    ((Response *)this)->req = req;
    ((Response *)this)->queue = queue;

}

static void Response_assemble(const void *this)
{

    printf("assembling response... ");
    Response *self = (Response *)this;
    unsigned short int headers_length = 0;
    for (unsigned short int i = 0; i < self->headers_count; i++) {
        headers_length += (strlen(self->headers[i]) + 1);
    }

    const size_t total_size = strlen(self->status) + 1 + headers_length + strlen(self->body) + 1;
    self->res_string = (char *) malloc(sizeof(char) * total_size);
    
    // printf("\nh-len: %d\n", headers_length);
    strcpy(self->res_string, self->status);
    strcat(self->res_string, "\n");
    for (unsigned short int i = 0; i < self->headers_count; i++) {
        strcat(self->res_string, self->headers[i]);
        strcat(self->res_string, "\n");
    }
    // strcat(self->res_string, "\n");
    strcat(self->res_string, self->body);
    printf("assembling done... \n");
    
    // printf("\n%s\n", self->res_string);
    
}

static char *Response_get(const void *this)
{
    return ((Response *)this)->res_string; 
}

static void Response_handle(const void *this)
{

    printf("Creating response... ");
    Response *self = (Response *)this;
    const Request *req = self->req;
    Queue *queue = self->queue;

    self->setHeaders(this);
    printf("headers added... ");
    // printf("meth: %s\n", req->method);

    // Consume.
    if (strcmp(req->method, "GET") == 0)
    {

        printf("recognized as GET... ");

        char *json_start = "{ \"msg\": \"";
        char *json_end = "\" }";
        char *msg;
        
        if (!_Queue.isEmpty(queue))
        {
            self->setStatus(this, 200);
            Node *node = _Queue.poll(queue);
            msg = (char *) malloc(sizeof(char) * strlen(_Node.getMessage(node)) + 1);
            strcpy(msg, _Node.getMessage(node));
            _Node.destruct(node);
        }
        else
        {
            self->setStatus(this, 204);
            char *queue_empty_msg = "Queue is empty.";
            msg = (char *) malloc(sizeof(char) * strlen(queue_empty_msg) + 1);
            strcpy(msg, queue_empty_msg);
        }

        size_t body_len = strlen(msg) + strlen(json_start) + strlen(json_end) + 1;
        self->body = (char *) malloc(sizeof(char) * body_len);
        strcpy(self->body, json_start);
        strcat(self->body, msg);
        strcat(self->body, json_end);
        free(msg);

    }
    // Produce.
    else if (strcmp(req->method, "POST") == 0)
    {

        printf("recognized as POST... ");

        self->setStatus(this, 200);
        
        Node *newNode = Node_new();
        _Node.setMessage(newNode, req->body->get(req->body, "message"), strlen(req->body->get(req->body, "message")));
        _Queue.add(queue, newNode);

        // printf("\n%s\n", self->status);

        const char *body = "{ \"msg\": \"Message produced.\" }";
        self->body = (char *) malloc(sizeof(char) * (strlen(body) + 1));
        strcpy(self->body, body);
        // printf("\n%s\n", self->body);

    }
    else
    {
        self->setStatus(this, 500);
        self->body = (char *) malloc(1);
        strcpy(self->body, "");
    } 

    printf("body added... ");
    
}

static void Response_set_status(const void *this, unsigned short int code)
{

    Response *self = (Response *)this;
    char *status_txt;

    switch(code)
    {
        case 200:
            status_txt = STATUS_200;
            break;
        case 204:
            status_txt = STATUS_204;
            break;
        case 500:
            status_txt = STATUS_500;
            break;
        default:
            status_txt = STATUS_200;
    }
    
    self->status = (char *) malloc(sizeof(char) * (strlen(PROTOCOL) + strlen(status_txt) + 2));
    strcpy(self->status, PROTOCOL);
    strcat(self->status, " ");
    strcat(self->status, status_txt);
    printf("status %d set... ", code);
   
}

static void Response_set_headers(const void *this)
{

    Response *self = (Response *)this;

    self->headers_count = 5;
    self->headers = (char **) malloc(sizeof(char *) * self->headers_count);
    // Alow origin header.
    self->headers[0] = (char *) malloc(sizeof(char) * (strlen(HEADER_ALLOW_ORIGIN) + 1));
    strcpy(self->headers[0], HEADER_ALLOW_ORIGIN);
    // Connection close header.
    self->headers[1] = (char *) malloc(sizeof(char) * (strlen(HEADER_CONNECTION_CLOSE + 1)));
    strcpy(self->headers[1], HEADER_CONNECTION_CLOSE);
    // Content type header.
    self->headers[2] = (char *) malloc(sizeof(char) * (strlen(HEADER_CONTENT_TYPE) + 1));
    strcpy(self->headers[2], HEADER_CONTENT_TYPE);
    // Host header.
    self->headers[3] = (char *) malloc(sizeof(char) * (strlen(HEADER_HOST) + 1));
    strcpy(self->headers[3], HEADER_HOST);

    // Date header.
    time_t now = time(&now);
    if (now == -1) {
        puts("The time() function failed");
    }
    struct tm *ptm = gmtime(&now);
    if (ptm == NULL) {
        puts("The gmtime() function failed");
    }    
    // printf("UTC time: %s", asctime(ptm));
    char *date_head = "Date: ";
    char *date = (char *) malloc(sizeof(char) * (strlen(date_head) + strlen(asctime(ptm)) + 1));
    strcpy(date, date_head);
    strcat(date, asctime(ptm));
    self->headers[4] = (char *) malloc(sizeof(char) * (strlen(date) + 1));
    strcpy(self->headers[4], date);
    free(date);

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