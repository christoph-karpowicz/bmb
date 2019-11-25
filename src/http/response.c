#include "response.h"

Response *Response_new(Request *req, Queue *queue) 
{

    Response *newResponse = (Response *) mem_alloc(sizeof(Response));

    // Assign methods.
    newResponse->construct = &Response_construct;
    newResponse->assemble = &Response_assemble;
    newResponse->get = &Response_get;
    newResponse->handle = &Response_handle;
    newResponse->handleGET = &Response_handle_GET;
    newResponse->handlePOST = &Response_handle_POST;
    newResponse->setHeaders = &Response_set_headers;
    newResponse->setStatus = &Response_set_status;
    newResponse->setSuccess = &Response_set_success;
    newResponse->destruct = &Response_destruct;

    newResponse->construct(newResponse, req, queue);
    return newResponse;

}

static void Response_construct(void *this, Request *req, Queue *queue) 
{

    printf("Response constructor called.\n");
    ((Response *)this)->status =        NULL;
    ((Response *)this)->headers =       NULL;
    ((Response *)this)->body =          NULL;
    ((Response *)this)->queue =         queue;
    ((Response *)this)->json_body =     cJSON_CreateObject();
    ((Response *)this)->req =           req;
    ((Response *)this)->success =       false;
    ((Response *)this)->error =         NULL;
    ((Response *)this)->res_string =    NULL;

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
    self->res_string = (char *) mem_alloc(total_size);
    
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

    Response *self =        (Response *)this;
    const Request *req =    self->req;
    Queue *queue =          self->queue;
    char *msg =             NULL;
    cJSON *json_msg =       NULL;
    cJSON *json_success =   NULL;
    bool handled = false;

    self->setHeaders(this);
    printf("headers added... ");

    if (req == NULL)
    {
        self->setStatus(this, 400);
        msg = (char *) mem_alloc(1);
        strcpy(msg, "");
        goto parseToJSON;
    }

    // GET request.
    if (strcmp(req->method, "GET") == 0) {
        handled = self->handleGET(this, &msg);
    }
    // POST request.
    else if (strcmp(req->method, "POST") == 0) {
        handled = self->handlePOST(this, &msg);
    }

    // ERROR - request type not found.
    if (msg == NULL) {
        self->setStatus(this, 400);
        msg = (char *) mem_alloc(1);
        strcpy(msg, "");
    }

    if (self->error != NULL) {
        self->setStatus(this, 500);
        msg = (char *) mem_alloc(strlen(self->error));
        strcpy(msg, self->error);
    }

    parseToJSON: ;
    json_success = cJSON_CreateBool(self->success);
    if (json_success == NULL)
    {
        exit(1);
    }
    cJSON_AddItemToObject(self->json_body, "success", json_success);
    json_msg = cJSON_CreateString(msg);
    if (json_msg == NULL)
    {
        exit(1);
    }
    cJSON_AddItemToObject(self->json_body, "data", json_msg);
    char *json_response_string = cJSON_Print(self->json_body);
    if (json_response_string == NULL) {
        exit(1);
    }

    size_t body_len = strlen(json_response_string) + 1;
    self->body = (char *) mem_alloc(sizeof(char) * body_len);
    strcpy(self->body, json_response_string);

    mem_free(msg);

    printf("body added... ");
    
}

static bool Response_handle_GET(const void *this, char **msg)
{
    printf("recognized as GET... ");

    Response *self =        (Response *)this;
    const Request *req =    self->req;
    Queue *queue =          self->queue;

    // Consume.
    if (strcmp(req->body->get(req->body, "type"), "consume") == 0) {
        if (!_Queue.isEmpty(queue)) {
            Node *node = _Queue.poll(queue);
            *msg = (char *) mem_alloc(strlen(_Node.getMessage(node)) + 1);
            strcpy(*msg, _Node.getMessage(node));
            _Node.destruct(node);
            self->setSuccess(this, true);
        }
        else {
            char *queue_empty_msg = "Queue is empty.";
            *msg = (char *) mem_alloc(strlen(queue_empty_msg) + 1);
            strcpy(*msg, queue_empty_msg);
        }
        self->setStatus(this, 200);
    }
    // Return queue length.
    else if (strcmp(req->body->get(req->body, "type"), "length") == 0) {
        const int queueSize = _Queue.size(queue);
        char *queueSize_str = intToString(queueSize);
        *msg = (char *) mem_alloc(strlen(queueSize_str) + 1);
        strcpy(*msg, queueSize_str);
        mem_free(queueSize_str);
        self->setStatus(this, 200);
        self->setSuccess(this, true);
    }
    // Peek queue Node at specified index.
    else if (strcmp(req->body->get(req->body, "type"), "get") == 0) {
        const int index =       atoi(req->body->get(req->body, "index"));
        const int queueSize =   _Queue.size(queue);
        if (queueSize > index) {
            Node *node = _Queue.get(queue, index);
            *msg = (char *) mem_alloc(strlen(_Node.getMessage(node)) + 1);
            strcpy(*msg, _Node.getMessage(node));
            self->setSuccess(this, true);
        }
        else {
            char *res = "Node index out of range.";
            *msg = (char *) mem_alloc(strlen(res) + 1);
            strcpy(*msg, res);
        }
        self->setStatus(this, 200);
    }
    // Peek all nodes.
    else if (strcmp(req->body->get(req->body, "type"), "getAll") == 0) {
        const int queueSize =   _Queue.size(queue);
        cJSON *nodes =          cJSON_CreateObject();
        cJSON *nodesArray =     NULL;
        char *nodesString =     NULL;
        
        nodesArray = cJSON_AddArrayToObject(nodes, "nodes");
        if (nodesArray == NULL) {
            self->setError(self, "Failed to add an array to JSON node list.");
            return false;
        }

        for (size_t i = 0; i < queueSize; i++) {
            Node *node =            _Queue.get(queue, i);
            char *nodeMessage =     _Node.getMessage(node);
            cJSON *nodeJSONObject = cJSON_CreateObject();
            cJSON *message =        NULL;

            if (cJSON_AddNumberToObject(nodeJSONObject, "index", i) == NULL) {
                self->setError(self, "Failed to add index to node JSON object.");
                return false;
            }

            if(cJSON_AddStringToObject(nodeJSONObject, "message", nodeMessage) == NULL) {
                self->setError(self, "Failed to add message to node JSON object.");
                return false;
            }

            cJSON_AddItemToArray(nodesArray, nodeJSONObject);
        }

        nodesString = cJSON_Print(nodes);
        if (nodesString == NULL) {
            self->setError(self, "Failed to print nodes.");
            return false;
        }
        
        *msg = (char *) mem_alloc(strlen(nodesString) + 1);
        strcpy(*msg, nodesString);
        cJSON_Delete(nodes);
        self->setStatus(this, 200);
        self->setSuccess(this, true);
    }

    return true;

}

static bool Response_handle_POST(const void *this, char **msg)
{
    printf("recognized as POST... ");

    Response *self = (Response *)this;
    const Request *req = self->req;
    Queue *queue = self->queue;

    // Produce.
    Node *newNode = Node_new();
    _Node.setMessage(newNode, req->body->get(req->body, "message"), strlen(req->body->get(req->body, "message")));
    _Queue.add(queue, newNode);

    char *res = "Message produced.";
    *msg = (char *) mem_alloc(strlen(res) + 1);
    strcpy(*msg, res);
    
    self->setStatus(this, 200);
    self->setSuccess(this, true);

    return true;
}

static void Response_set_error(const void *this, const char *msg)
{
    Response *self = (Response *)this;
    self->error = (char *) mem_alloc(strlen(msg));
    strcpy(self->error, msg);
}

static void Response_set_headers(const void *this)
{

    Response *self = (Response *)this;

    self->headers_count = 5;
    self->headers = (char **) mem_alloc(sizeof(char *) * self->headers_count);
    // Alow origin header.
    self->headers[0] = (char *) mem_alloc(strlen(HEADER_ALLOW_ORIGIN) + 1);
    strcpy(self->headers[0], HEADER_ALLOW_ORIGIN);
    // Connection close header.
    self->headers[1] = (char *) mem_alloc(strlen(HEADER_CONNECTION_CLOSE + 1));
    strcpy(self->headers[1], HEADER_CONNECTION_CLOSE);
    // Content type header.
    self->headers[2] = (char *) mem_alloc(strlen(HEADER_CONTENT_TYPE) + 1);
    strcpy(self->headers[2], HEADER_CONTENT_TYPE);
    // Host header.
    self->headers[3] = (char *) mem_alloc(strlen(HEADER_HOST) + 1);
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
    char *date = (char *) mem_alloc(strlen(date_head) + strlen(asctime(ptm)) + 1);
    strcpy(date, date_head);
    strcat(date, asctime(ptm));
    self->headers[4] = (char *) mem_alloc(strlen(date) + 1);
    strcpy(self->headers[4], date);
    mem_free(date);

}

static void Response_set_status(const void *this, unsigned short int code)
{

    char                *status_txt;
    Response *self =    (Response *)this;

    switch(code)
    {
        case 200:
            status_txt = STATUS_200;
            break;
        case 204:
            status_txt = STATUS_204;
            break;
        case 400:
            status_txt = STATUS_400;
            break;
        case 500:
            status_txt = STATUS_500;
            break;
        default:
            status_txt = STATUS_200;
    }
    
    self->status = (char *) mem_alloc(strlen(PROTOCOL) + strlen(status_txt) + 2);
    strcpy(self->status, PROTOCOL);
    strcat(self->status, " ");
    strcat(self->status, status_txt);
    printf("status %d set... ", code);
   
}

static void Response_set_success(const void *this, bool success)
{
    ((Response *)this)->success = success;
}

static void Response_destruct(void *this)
{
    Response *self = (Response *)this;
    mem_free(self->status);
    for (unsigned short int i = 0; i < self->headers_count; i++) {
        mem_free(self->headers[i]);
    }
    mem_free(self->headers);
    mem_free(self->body);
    mem_free(self->error);
    mem_free(self->res_string);
    cJSON_Delete(self->json_body);
    mem_free(this);
}