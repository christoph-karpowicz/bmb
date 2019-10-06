#include "request.h"

Request *Request_parse(const char *req) 
{

    Request *request = (Request *) malloc(sizeof(Request));

    printf("Parsing request... ");

    char **method = Request_extract_method(req);
    printf("extracted method: %s... ", *method);

    request->method = (char *) malloc(sizeof(char) * (strlen(*method) + 1));
    strcpy(request->method, *method);
    free(*method);

    if (strcmp(request->method, "POST") == 0) {
        
        RequestPair *messagePair = Request_extract_data(req, "message");
        printf("extracted message: %s\n", messagePair->getValue(messagePair));
        RequestPair *typePair = Request_extract_data(req, "type");
        printf("extracted type: %s\n", typePair->getValue(typePair));
        RequestPair *queuePair = Request_extract_data(req, "queue");
        printf("extracted queue: %s\n", queuePair->getValue(queuePair));
        
        request->message = messagePair->getValue(messagePair);
    }
    else if (strcmp(request->method, "GET") == 0) {
        
        RequestPair *typePair = Request_extract_data(req, "type");
        printf("extracted type: %s\n", typePair->getValue(typePair));
        RequestPair *queuePair = Request_extract_data(req, "queue");
        printf("extracted queue: %s\n", queuePair->getValue(queuePair));
        
    }

    return request;
    
}

RequestPair *Request_extract_data(const char *request, const char *key) 
{

    regex_t regex;
    // Match structure for 3 subexpressions.
    const unsigned short int subexNo = 3;
    regmatch_t rm[subexNo];
    unsigned short int reti;
    char msgbuf[100];
    char *data = NULL;

    // Compile regex.
    char *regex_pattern_start = "(";

    char *regex_pattern_end;
    if (strcmp(key, "queue") == 0)
        regex_pattern_end = "=)([0-9]*)";
    else
        regex_pattern_end = "=)([^&]*)";

    char *regex_pattern = (char *) malloc(sizeof(char) * (strlen(regex_pattern_start) + strlen(key) + strlen(regex_pattern_end) + 1));
    // char *regex_pattern = "(data=)(.*)(&|$)";
    strcpy(regex_pattern, regex_pattern_start);
    strcat(regex_pattern, key);
    strcat(regex_pattern, regex_pattern_end);
    reti = regcomp(&regex, regex_pattern, REG_EXTENDED);
    free(regex_pattern);
    if (reti)
    { 
        fprintf(stderr, "Could not compile regex\n");
        exit(1); 
    }

    // Execute regex.
    reti = regexec(&regex, request, subexNo, rm, 0);
    if (!reti)
    {

        unsigned short int start = rm[2].rm_so;
        unsigned short int finish = rm[2].rm_eo;
        unsigned short int diff = finish - start;

        if (diff <= 0) return NULL;
        
        data = (char *) malloc(sizeof(char) * (diff + 1));
        strncpy(data, &request[start], diff);

        // printf("Match, start: %d, finish: %d\n", start, finish);
        
    }
    else if (reti == REG_NOMATCH)
    {
        puts("No match");
        return NULL;
    }
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        exit(1);
    }

    // Free compiled regex.
    regfree(&regex);

    RequestPair *reqPair = RequestPair_new(key, data);

    return reqPair;

}

char **Request_extract_method(const char *request) 
{

    const unsigned short int requestLength = strlen(request);
    if (requestLength == 0) return NULL;
    
    unsigned short int i = 0;
    const char delimiter = ' ';
    char *method = (char *) malloc(0);
    char **method_ptr;

    while (request[i] != delimiter) 
    {
        method = (char *) realloc(method, sizeof(char) * (i + 2));
        method[i] = request[i];
        i++;
    }
    method[i] = '\0';

    method_ptr = &method;
    return method_ptr;

}

void Request_destruct(Request *this)
{
    if (strcmp(this->method, "POST") == 0)
        free(this->message);
    free(this->method);
    free(this);
}