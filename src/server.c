#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "server.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void Server_init(Server* s) {

    s->client_message[CLIENT_MESSAGE_SIZE] = "\0";
    s->buffer[BUFFER_SIZE] = "\0";
    
    // Configure settings of the server address struct
    // Address family = Internet 
    s->serverAddr.sin_family = AF_INET;

    //Set port number, using htons function to use proper byte order 
    s->serverAddr.sin_port = htons(PORT);

    //Set IP address to localhost 
    s->serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Set all bits of the padding field to 0 
    memset(s->serverAddr.sin_zero, '\0', sizeof s->serverAddr.sin_zero);

}

bool Server_create_socket(Server* s) {

    s->socket = socket(PF_INET, SOCK_STREAM, 0);
    if (s->socket < 0) {
        error("ERROR opening socket");
        return false;
    }
    return true;

}

bool Server_bind(Server* s) {
    
    //Bind the address struct to the socket 
    if (bind(s->socket, (struct sockaddr *) &s->serverAddr, sizeof(s->serverAddr)) < 0) {
        error("ERROR on binding");
        return false;
    }
    return true;
    
}