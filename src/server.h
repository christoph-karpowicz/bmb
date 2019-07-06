#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define CLIENT_MESSAGE_SIZE 2000

typedef struct Server {
    char* client_message[CLIENT_MESSAGE_SIZE];
    char* buffer[BUFFER_SIZE];
    int socket;
    int newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

} Server;

void Server_init(Server* s);

bool Server_create_socket(Server* s);

bool Server_bind(Server* s);

typedef struct socket_thread_arguments {
    int* socket;
    char* client_message;
    char* buffer;
} socket_thread_args;

void error(const char *msg);

#endif