#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>

#include "../queue/queue.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define CLIENT_MESSAGE_SIZE 2000

typedef struct Server {

    // Properties.
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

bool Server_bind(const Server *s);

bool Server_create_socket(Server *s);

void Server_init(Server *s);

typedef struct socket_thread_arguments {
    int *socket;
    Queue *queue;
    unsigned long long time_start;
} socket_thread_args;

void Server_error(const char *msg);

#endif