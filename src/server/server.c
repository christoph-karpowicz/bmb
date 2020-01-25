#include "server.h"

// Global server instance pointer initialization.
Server *server = NULL;

/**
 * Server_error - print error to stderr and terminate the application.
 * @msg: error message
 */
void Server_error(const char *msg)
{
    perror(msg);
    exit(1);
}

/**
 * Server_accept - creates a new connected socket for the incoming connection.
 * @s: server instance
 * 
 * RETURNS:
 * was the connected socket created successfully
 */
int Server_accept(Server *s, Connection *conn) {
    conn->addr_size = sizeof conn->addr;
    int socketFd = accept(s->socket, (struct sockaddr *)&conn->addr, &conn->addr_size);
    if (socketFd < 0) {
        s->error("Error on accept");
    }
    ++s->requestCounter;
    return socketFd;
}

/**
 * Server_bind - binds the address struct to the socket.
 * @s: server instance
 * 
 * RETURNS:
 * was an address successfully bound to the socket
 */
bool Server_bind(const Server *s) {
    if (bind(s->socket, (struct sockaddr *)&s->serverAddr, sizeof(s->serverAddr)) < 0) {
        s->error("Error on binding");
        return false;
    }
    return true;
    
}

/**
 * Server_create_socket - creates an endpoint for communication
 * and assigns a file descriptor to the socket Server struct member.
 * @s: server instance
 * 
 * RETURNS:
 * was the socket created successfully
 */
bool Server_create_socket(Server *s) {

    s->socket = socket(PF_INET, SOCK_STREAM, 0);
    if (s->socket < 0) {
        s->error("Error opening socket");
        return false;
    }
    int optTrue = 1;
    setsockopt(s->socket, SOL_SOCKET, SO_REUSEADDR, &optTrue, sizeof(int));
    // setsockopt(s->socket, SOL_SOCKET, SO_DEBUG, &optTrue, sizeof(int));
    return true;

}

/**
 * Server_init - initializes a server instance.
 * @s: server instance
 * 
 * Assigns socket address struct members,
 * creates a broker instance and
 * starts the connection count.
 */
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

    // Create broker.
    s->broker = broker_init();

    // Start request counter.
    s->requestCounter = 0;

}

void Server_destruct(Server *s) {
    mem_free(s->broker);
    mem_free(s);
}