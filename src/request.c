#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "request.h"

Request *Request_parse(const char *req) 
{

    Request *request = (Request *) malloc(sizeof(Request));

    printf("Create request.\n");

    // char *message = Request_extract_message(req);
    // printf("message: %s\n", message);
    // printf("message addr: %p\n", &*message);
    // request->message = message;
    // printf("message addr: %p\n", &*request->message);
    char **message = Request_extract_message(req);
    printf("message: %s\n", *message);
    printf("message addr: %p\n", &(**message));
    printf("message: %s\n", *message);
    printf("message addr: %p\n", &(**message));
    printf("message len: %d\n", strlen(*message));
    printf("message: %s\n", *message);
    request->message = (char *) malloc(sizeof(char) * (strlen(*message) + 1));
    printf("tst: %d\n", 2);
    strcpy(request->message, *message);
    // request->message[strlen(*message)] = '\0';
    if (!strcmp(*message, ""))
        free(*message);


    char **method = Request_extract_method(req);
    printf("method: %s\n", *method);
    // printf("meth addr: %p\n", &*method);
    // printf("meth len: %d\n", strlen(*method));

    // request->method = method;
    request->method = (char *) malloc(sizeof(char) * (strlen(*method) + 1));
    strcpy(request->method, *method);
    free(*method);
    // free(**method);

    return request;
    
}

char **Request_extract_message(const char *request) 
{

    regex_t regex;
    // Match structure for 3 subexpressions.
    const unsigned short int subexNo = 3;
    regmatch_t rm[subexNo];
    unsigned short int reti;
    char msgbuf[100];
    char *message = (char *) malloc(0);
    char **message_ptr = NULL;
    char *empty_str = "";

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

        if (diff <= 0)
        {
            // return "";
            message_ptr = &empty_str;
            return message_ptr;
        }
        
        printf("diff: %d\n", diff);
        message = (char *) realloc(message, sizeof(char) * (diff + 1));
        strncpy(message, &request[start], diff + 1);

        printf("c: %s\n", &request[start]);
        printf("m: %s\n", message);
        printf("l: %d\n", strlen(message));
        int i = 0;
        while(message[i] != '\0'){
            printf("c: %c\n", message[i]);
            if (i==1000) break;
            i++;
        }
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

    printf("fff: message addr: %p\n", &(*message));
    message_ptr = &(*message);
    printf("fff: message addr: %p\n", &(**message_ptr));
    return message_ptr;
    // return message;

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
        // strcat(method, request[i]);
        method[i] = request[i];
        i++;
    }
    method[i] = '\0';

    // memcpy(method, request, i);

    // printf("---------------\n");
    // printf("meth: %s\n", method);
    // printf("meth addr: %p\n", &method);
    // printf("---------------\n");
    // printf("len: %d\n", strlen(method));

    method_ptr = &method;
    return method_ptr;

}

void Request_destruct(Request *this)
{
    free(this->message);
    free(this->method);
    free(this);
}