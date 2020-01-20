#include "response.h"

/**
 * Response_new - Response struct constructor.
 * @req: Request struct with http data and method
 * @broker: broker API
 * 
 * This struct processes the http request 
 * and gathers the requested data from the broker.
 * 
 * RETURNS:
 * new Response struct
 */
Response *Response_new(Request *req, Broker *broker) 
{
    Response *newResponse = (Response *) mem_alloc(sizeof(Response));

    // Assign methods.
    newResponse->construct = &Response_construct;
    newResponse->assemble = &Response_assemble;
    newResponse->get = &Response_get;
    newResponse->getResponse = &Response_get_response;
    newResponse->handle = &Response_handle;
    newResponse->setError = &Response_set_error;
    newResponse->setHeaders = &Response_set_headers;
    newResponse->setStatus = &Response_set_status;
    newResponse->destruct = &Response_destruct;

    newResponse->construct(newResponse, req, broker);
    return newResponse;
}

// @todo: to be removed
static void Response_construct(void *this, Request *req, Broker *broker) 
{
    printf("Response constructor called.\n");
    ((Response *)this)->status      = NULL;
    ((Response *)this)->headers     = NULL;
    ((Response *)this)->body        = NULL;
    ((Response *)this)->broker      = broker;
    ((Response *)this)->json_body   = cJSON_CreateObject();
    ((Response *)this)->req         = req;
    ((Response *)this)->res_string  = NULL;
}

/**
 * Response_assemble - joins headers, status and the response body.
 * @this: Response instance
 */
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

/**
 * Response_get
 * @this: Response instance
 * 
 * RETURNS:
 * http response string
 */
static char *Response_get(const void *this)
{
    return ((Response *)this)->res_string; 
}

/**
 * Response_get_response - sends a request to the broker API.
 * @this: Response instance
 * 
 * RETURNS:
 * broker_response struct contaning the requested data
 * or a result of an action on one of the queues.
 */
static struct broker_response Response_get_response(const void *this)
{
    Response *self          = (Response *)this;
    const Request *req      = self->req;
    struct broker_request   broker_req;
    struct broker_response  res;

    broker_req.type         = req->body->get(req->body, "type");
    broker_req.queue        = NULL;
    broker_req.queueName    = req->body->get(req->body, "queue");
    broker_req.index        = req->body->get(req->body, "index") == NULL ? 0 : atoi(req->body->get(req->body, "index"));
    broker_req.data         = req->body->get(req->body, "data");

    res = broker_dispatch(self->broker, broker_req);

    return res;
}

/**
 * Response_handle - main method, doing most of the response creation work.
 * @this: Response instance
 * 
 * Makes sure that the request struct was creates successfully, 
 * adds headers to the final result,
 * queries teh broker API
 * creates a JSON body with received data.
 */
static void Response_handle(const void *this)
{
    printf("Creating response... ");

    Response *self          = (Response *)this;
    const Request *req      = self->req;
    struct broker_response  broker_res;
    char                    *errMsg;

    self->setHeaders(this);
    printf("headers added... ");

    if (req == NULL)
    {
        self->setStatus(this, 400);
        errMsg = "Empty request received.";
        goto errorResponse;
    }

    printf("recognized as %s... ", req->method);

    // Create response.
    broker_res = self->getResponse(this);

    self->setStatus(this, broker_res.code);

    cJSON_AddItemToObject(self->json_body, "success", broker_res.success);
    cJSON_AddItemToObject(self->json_body, "payload", broker_res.payload);
    char *json_response_string = cJSON_Print(self->json_body);
    if (json_response_string == NULL) {
        self->setStatus(this, 500);
        errMsg = "Failed to create final JSON response.";
        goto errorResponse;
    }

    size_t body_len = strlen(json_response_string) + 1;
    self->body = (char *) mem_alloc(sizeof(char) * body_len);
    strcpy(self->body, json_response_string);

    printf("body added... ");

    return;
    
    errorResponse:
    printf("error response added... ");
    self->setError(self, errMsg);
    
    return;
}

/**
 * Response_set_error - sets an error message in the response body.
 * @this: Response instance
 * @errorMessage: error string
 */
static void Response_set_error(const void *this, char *errorMessage)
{
    Response *self  = (Response *)this;
    size_t body_len = strlen(errorMessage) + 1;

    mem_free(self->body);
    self->body = (char *) mem_alloc(sizeof(char) * body_len);

    strcpy(self->body, errorMessage);
}

/**
 * Response_set_headers - sets a fixed number of headers to the response.
 * @this: Response instance
 */
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

/**
 * Response_set_status - sets the http response code.
 * @this: Response instance
 * @code: http response code
 */
static void Response_set_status(const void *this, unsigned short int code)
{
    char                *status_txt;
    Response *self =    (Response *)this;

    switch(code) {
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

static void Response_destruct(void *this)
{
    Response *self = (Response *)this;
    mem_free(self->status);
    for (unsigned short int i = 0; i < self->headers_count; i++) {
        mem_free(self->headers[i]);
    }
    mem_free(self->headers);
    mem_free(self->body);
    mem_free(self->res_string);
    cJSON_Delete(self->json_body);
    mem_free(this);
}