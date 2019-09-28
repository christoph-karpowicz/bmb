#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

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
        char *message = Request_extract_message(req);
        printf("extracted message: %s\n", message);
        request->message = message;
    }

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
    char *message = NULL;

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
        strncpy(message, &request[start], diff + 1);

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
    if (strcmp(this->method, "POST") != 0)
        free(this->message);
    printf("%%%%%%%%%%%%%%%\n");
    free(this->method);
    free(this);
}