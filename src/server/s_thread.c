#include "s_thread.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * broker_thread - starting point for all request processing.
 * @args: argument passed into the newly created thread
 * 
 * Each accepted client request is being processed on a separate thread.
 * This function parses the http request string, queries the broker API, 
 * which queries the persistence API and creates a http reponse string.
 * The thread is closed after sending the response.
 */
void *broker_thread(void *args)
{
    log("Broker thread opened.\n");

    char msg[CLIENT_MESSAGE_SIZE];
    socket_thread_args *arguments   = (socket_thread_args *)args;
    Server *server                  = arguments->server;
    int socketFd                    = arguments->conn->socket_fd;

    // Read client message.
    recv(socketFd, msg, CLIENT_MESSAGE_SIZE, 0);
    
    // Receive and parse request.
    Request *req = Request_parse(msg);

    Response *res = Response_new(req, server->broker);
    
    // Lock thread during IO operations on the queue.
    pthread_mutex_lock(&lock);
    res->handle(res);
    pthread_mutex_unlock(&lock);
    
    res->assemble(res);

    // log("============\nReq:\n%s\n==========\n", msg);
    // log("============\nRes:\n%s\n==========\n", res->get(res));

    if (req != NULL)
        Request_destruct(req);

    send(socketFd, res->get(res), strlen(res->get(res)), 0);
    res->destruct(res);

    close(socketFd);

    log("Exit broker thread.\n");
    mem_free(arguments->conn);
    mem_free(arguments);
    pthread_exit(NULL);
}