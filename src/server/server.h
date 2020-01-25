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

#include "../broker/broker.h"
#include "../util/mem.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define CLIENT_MESSAGE_SIZE 2000

typedef struct {
    // Properties.
    int socket;
    struct sockaddr_in serverAddr;
    Broker *broker;
    int requestCounter;
    
    // Methods.
    void (*error)(const char *msg);
} Server;

typedef struct {
    int socket_fd;
    struct sockaddr addr;
    socklen_t addr_size;
} Connection;

extern Server *server;

int Server_accept(Server *s, Connection *conn);

bool Server_bind(const Server *s);

bool Server_create_socket(Server *s);

void Server_init(Server *s);

typedef struct {
    Connection *conn;
    Server *server;
    unsigned long long time_start;
} socket_thread_args;

void Server_destruct(Server *s);

void Server_error(const char *msg);

#endif