#ifndef REQUEST_H
#define REQUEST_H

typedef struct request 
{

    char *method;
    char *message;
    
} Request;

Request *Request_parse(const char *req);

char *Request_extract_message(const char *request);

char *Request_extract_method(const char *request);

void Request_destruct(Request *this);

#endif