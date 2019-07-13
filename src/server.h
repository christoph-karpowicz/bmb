#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>

#include "queue.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define CLIENT_MESSAGE_SIZE 2000

typedef struct Server {

    // Properties.
    char *client_message[CLIENT_MESSAGE_SIZE];
    char *buffer[BUFFER_SIZE];
    int socket;
    int newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    Queue *queue;
    int *requestCounter;
    
    // Methods.
    void (*error)(const char *msg);

} Server;

bool Server_accept(Server *s);

bool Server_bind(Server *s);

bool Server_create_socket(Server *s);

void Server_init(Server *s);

typedef struct socket_thread_arguments {
    int *socket;
    char *client_message;
    char *buffer;
    Queue *queue;
} socket_thread_args;

void error(const char *msg);

#endif