#include "response.h"

Response *Response_new(Request *req, Queue *queue) 
{

    Response *newResponse = (Response *) malloc(sizeof(Response));

    // Assign methods.
    newResponse->construct = &Response_construct;
    newResponse->assemble = &Response_assemble;
    newResponse->get = &Response_get;
    newResponse->handle = &Response_handle;
    newResponse->handleGET = &Response_handle_GET;
    newResponse->handlePOST = &Response_handle_POST;
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
    ((Response *)this)->json_body = cJSON_CreateObject();

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
    self->res_string = (char *) malloc(total_size);
    
    strcpy(self->res_string, self->status);
    strcat(self->res_string, "\n");
    for (unsigned short int i = 0; i < self->headers_count; i++) {
        strcat(self->res_string, self->headers[i]);
        strcat(self->res_string, "\n");
    }
    strcat(self->res_string, self->body);
    printf("assembling done... \n");
    
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

    if (req == NULL)
    {
        self->setStatus(this, 500);
        self->body = (char *) malloc(1);
        strcpy(self->body, "");
        return;
    }

    char *msg = NULL;

    // GET request.
    if (strcmp(req->method, "GET") == 0) {
        self->handleGET(this, &msg);
    }
    // POST request.
    else if (strcmp(req->method, "POST") == 0) {
        self->handlePOST(this, &msg);
    }

    // printf("#####msg: %s\n", msg);

    // ERROR - request type not found.
    if (msg == NULL) {
        self->setStatus(this, 500);
        msg = (char *) malloc(1);
        strcpy(msg, "");
    }

    cJSON *json_msg = NULL;
    json_msg = cJSON_CreateString(msg);
    if (json_msg == NULL)
    {
        // goto end;
    }
    cJSON_AddItemToObject(self->json_body, "msg", json_msg);
    char *json_response_string = cJSON_Print(self->json_body);
    size_t body_len = strlen(json_response_string) + 1;
    self->body = (char *) malloc(sizeof(char) * body_len);
    strcpy(self->body, json_response_string);

    free(msg);

    printf("body added... ");
    
}

static void Response_handle_GET(const void *this, char **msg)
{
    printf("recognized as GET... ");

    Response *self = (Response *)this;
    const Request *req = self->req;
    Queue *queue = self->queue;

    // Consume.
    if (strcmp(req->body->get(req->body, "type"), "consume") == 0)
    {
        if (!_Queue.isEmpty(queue))
        {
            self->setStatus(this, 200);
            Node *node = _Queue.poll(queue);
            *msg = (char *) malloc(strlen(_Node.getMessage(node)) + 1);
            strcpy(*msg, _Node.getMessage(node));
            _Node.destruct(node);
        }
        else
        {
            self->setStatus(this, 200);
            char *queue_empty_msg = "Queue is empty.";
            *msg = (char *) malloc(strlen(queue_empty_msg) + 1);
            strcpy(*msg, queue_empty_msg);
        }
    }
    // Return queue length.
    else if (strcmp(req->body->get(req->body, "type"), "length") == 0)
    {
        self->setStatus(this, 200);
        const int queueSize = _Queue.size(queue);
        char *queueSize_str = intToString(queueSize);
        *msg = (char *) malloc(strlen(queueSize_str) + 1);
        strcpy(*msg, queueSize_str);
        free(queueSize_str);
    }
    // Peek queue Node at specified index.
    else if (strcmp(req->body->get(req->body, "type"), "peek") == 0)
    {
        self->setStatus(this, 200);
        const int index = atoi(req->body->get(req->body, "index"));
        const int queueSize = _Queue.size(queue);
        // printf("size: %d, i: %d\n", queueSize, index);
        if (queueSize > index)
        {
            Node *node = _Queue.get(queue, index);
            *msg = (char *) malloc(strlen(_Node.getMessage(node)) + 1);
            strcpy(*msg, _Node.getMessage(node));
        }
        else 
        {
            char *res = "Node index out of range.";
            *msg = (char *) malloc(strlen(res) + 1);
            strcpy(*msg, res);
        }
    }

}

static void Response_handle_POST(const void *this, char **msg)
{
    printf("recognized as POST... ");

    Response *self = (Response *)this;
    const Request *req = self->req;
    Queue *queue = self->queue;

    self->setStatus(this, 200);
    
    // Produce.
    Node *newNode = Node_new();
    _Node.setMessage(newNode, req->body->get(req->body, "message"), strlen(req->body->get(req->body, "message")));
    _Queue.add(queue, newNode);

    char *res = "Message produced.";
    *msg = (char *) malloc(strlen(res) + 1);
    strcpy(*msg, res);
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
    
    self->status = (char *) malloc(strlen(PROTOCOL) + strlen(status_txt) + 2);
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
    self->headers[0] = (char *) malloc(strlen(HEADER_ALLOW_ORIGIN) + 1);
    strcpy(self->headers[0], HEADER_ALLOW_ORIGIN);
    // Connection close header.
    self->headers[1] = (char *) malloc(strlen(HEADER_CONNECTION_CLOSE + 1));
    strcpy(self->headers[1], HEADER_CONNECTION_CLOSE);
    // Content type header.
    self->headers[2] = (char *) malloc(strlen(HEADER_CONTENT_TYPE) + 1);
    strcpy(self->headers[2], HEADER_CONTENT_TYPE);
    // Host header.
    self->headers[3] = (char *) malloc(strlen(HEADER_HOST) + 1);
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
    char *date = (char *) malloc(strlen(date_head) + strlen(asctime(ptm)) + 1);
    strcpy(date, date_head);
    strcat(date, asctime(ptm));
    self->headers[4] = (char *) malloc(strlen(date) + 1);
    strcpy(self->headers[4], date);
    free(date);

}

static void Response_destruct(void *this)
{
    Response *self = (Response *)this;
    free(self->status);
    for (unsigned short int i = 0; i < self->headers_count; i++) {
        free(self->headers[i]);
    }
    free(self->headers);
    free(self->body);
    free(self->res_string);
    cJSON_Delete(self->json_body);
    free(this);
}