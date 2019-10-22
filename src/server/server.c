#include "server.h"

void Server_error(const char *msg)
{
    perror(msg);
    exit(1);
}

bool Server_accept(Server *s) {
    // Accept call creates a new socket for the incoming connection.
    s->addr_size = sizeof s->serverStorage;
    s->newSocket = accept(s->socket, (struct sockaddr *) &s->serverStorage, &s->addr_size);
    if (s->newSocket < 0) {
        s->error("ERROR on accept");
        return false;
    }
    ++*s->requestCounter;
    return true;
}

bool Server_bind(const Server *s) {
    
    // Bind the address struct to the socket.
    if (bind(s->socket, (struct sockaddr *) &s->serverAddr, sizeof(s->serverAddr)) < 0) {
        s->error("ERROR on binding");
        return false;
    }
    return true;
    
}

bool Server_create_socket(Server *s) {

    s->socket = socket(PF_INET, SOCK_STREAM, 0);
    if (s->socket < 0) {
        s->error("ERROR opening socket");
        return false;
    }
    return true;

}

void Server_init(Server *s) {

    // Assign methods.
    s->error = Server_error;

    // Configure settings of the server address struct
    // Address family = Internet 
    s->serverAddr.sin_family = AF_INET;

    //Set port number, using htons function to use proper byte order 
    s->serverAddr.sin_port = htons(PORT);

    //Set IP address to localhost 
    s->serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Set all bits of the padding field to 0 
    memset(s->serverAddr.sin_zero, '\0', sizeof s->serverAddr.sin_zero);

    // Create queue.
    s->queue = Queue_new();

    // Start request counter.
    s->requestCounter = (int *) malloc(sizeof(int));
    *s->requestCounter = 0;

}