#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "request.h"

Request *Request_parse(const char *req) 
{

    Request *request = (Request *) malloc(sizeof(Request));

    printf("Create request.\n");

    char *method = Request_extract_method(req);
    printf("method: %s\n", method);
    // free(method);

    char *message = Request_extract_message(req);
    printf("message: %s\n", message);
    // free(message);

    request->method = method;
    request->message = message;

    return request;
    
}

char *Request_extract_message(const char *request) 
{

    regex_t regex;
    // Match structure for 3 subexpressions.
    const unsigned short int subexNo = 3;
    regmatch_t rm[subexNo];
    unsigned short int reti;
    char msgbuf[100];
    char *message;

    // Compile regex.
    reti = regcomp(&regex, "(message=)(.*)$", REG_EXTENDED);
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

        if (diff <= 0) return "";
        
        message = (char *) malloc(sizeof(char) * (diff + 1));
        memcpy(message, &request[start], diff);
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

    return message;

}

char *Request_extract_method(const char *request) 
{

    const unsigned short int requestLength = strlen(request);
    if (requestLength == 0) return NULL;
    
    unsigned short int i = 0;
    const char delimiter = ' ';

    while (request[i] != delimiter)
        i++;

    char *method = (char *) malloc(sizeof(char) * (i + 1));

    memcpy(method, request, i);

    return method;

}

void Request_destruct(Request *this)
{
    free(this->message);
    free(this->method);
    free(this);
}