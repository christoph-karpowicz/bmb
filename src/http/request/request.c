#include "request.h"

/**
 * Request_parse - parses a request string
 * 1. extracts an http method,
 * 2. extracts the GET/POST params from the request,
 * 3. adds and stores all extracted param=>value pairs in a RequestBody struct.
 * @req: http request string read from a socket
 * 
 * RETURNS:
 * new Request struct for further processing
 */
Request *Request_parse(const char *req) 
{
    printf("Parsing request... ");
    Request *request    = (Request *) mem_alloc(sizeof(Request));
    request->method     = NULL;
    request->body       = RequestBody_new();

    char *method = Request_extract_method(req);
    printf("extracted method: %s... ", method); 

    if (method == NULL)
    {
        Request_destruct(request);
        return NULL;
    } 

    request->method = method;

    RequestPair *typePair = Request_extract_data(req, "type");
    if (typePair != NULL) {
        printf("extracted type: %s\n", typePair->getValue(typePair));
        request->body->add(request->body, typePair);
    }

    RequestPair *queuePair = Request_extract_data(req, "queue");
    if (queuePair != NULL) {
        printf("extracted queue: %s\n", queuePair->getValue(queuePair));
        request->body->add(request->body, queuePair);
    }

    if (strcmp(request->method, "POST") == 0) {
        
        RequestPair *dataPair = Request_extract_data(req, "data");
        if (dataPair != NULL) {
            printf("extracted data: %s\n", dataPair->getValue(dataPair));
            request->body->add(request->body, dataPair);
        }

    }
    else if (strcmp(request->method, "GET") == 0) {
        
        RequestPair *indexPair = Request_extract_data(req, "index");
        if (indexPair != NULL)
        {
            printf("extracted index: %s\n", indexPair->getValue(indexPair));
            request->body->add(request->body, indexPair);
        }

    }

    return request;
}

/**
 * Request_extract_data - used by Request_parse, extracts the GET/POST params from the request.
 * @request: http request string passed from Request_parse
 * @key: param name
 * 
 * RETURNS:
 * new RequestPair (param => value)
 */
static RequestPair *Request_extract_data(const char *request, const char *key) 
{

    regex_t regex;
    // Match structure for 3 subexpressions.
    const unsigned short int subexNo = 3;
    regmatch_t rm[subexNo];
    unsigned short int reti;
    char *data = NULL;
    bool no_match = false;

    // Compile regex.
    char *regex_pattern_start = "(";
    char *regex_pattern_end = "=)([^&[:space:]]*)";

    char *regex_pattern = (char *) mem_alloc(sizeof(char) * (strlen(regex_pattern_start) + strlen(key) + strlen(regex_pattern_end) + 1));
    strcpy(regex_pattern, regex_pattern_start);
    strcat(regex_pattern, key);
    strcat(regex_pattern, regex_pattern_end);
    reti = regcomp(&regex, regex_pattern, REG_EXTENDED);
    mem_free(regex_pattern);
    
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1); 
    }

    // Execute regex.
    reti = regexec(&regex, request, subexNo, rm, 0);
    if (!reti) {

        unsigned short int start = rm[2].rm_so;
        unsigned short int finish = rm[2].rm_eo;
        unsigned short int diff = finish - start;

        if (diff <= 0) return NULL;
        
        data = (char *) mem_alloc(sizeof(char) * (diff + 1));
        strncpy(data, &request[start], diff);
        data[diff] = '\0';

        // printf("=======***** Diff: %d, data: %s\n", diff, data);
        
    }
    else if (reti == REG_NOMATCH) {
        no_match = true;
        printf("No match for %s.\n", key);
    }
    else {
        char msgbuf[100];
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        exit(1);
    }

    // Free compiled regex.
    regfree(&regex);

    if (no_match) return NULL;

    RequestPair *reqPair = RequestPair_new(key, data);

    return reqPair;

}

/**
 * Request_extract_method - extracts an http method.
 * @request: http request string passed from Request_parse
 * 
 * RETURNS:
 * http method as string
 */
static char *Request_extract_method(const char *request) 
{

    const unsigned short int requestLength = strlen(request);
    if (requestLength == 0) return NULL;

    unsigned short int i = 0;
    const char delimiter = ' ';
    char *method = NULL;

    while (request[i] != delimiter) 
    {
        size_t methodMemSize = sizeof(char) * (i + 2);
        if (method == NULL)
            method = (char *) mem_alloc(methodMemSize);
        else
            method = (char *) realloc(method, methodMemSize);
        method[i] = request[i];
        i++;
    }
    method[i] = '\0';

    return method;

}

void Request_destruct(Request *this)
{
    if (this->body != NULL)
        this->body->destruct(this->body);
    mem_free(this->method);
    mem_free(this);
}