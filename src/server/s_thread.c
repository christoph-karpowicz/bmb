#include "s_thread.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * socketThread - starting point for all request processing.
 * @args: argument passed into the newly created thread
 * 
 * Each accepted client request is being processed on a separate thread.
 * This function parses the http request string, queries the broker API, 
 * which queries the persistence API and creates a http reponse string.
 * The thread is closed after sending the response.
 */
void *socketThread(void *args)
{
    printf("Socket thread opened.\n");

    socket_thread_args *arguments = (socket_thread_args*)args;
    Server *server =                arguments->server;
    int *newSocketPtr =             &server->newSocket;
    char                            msg[CLIENT_MESSAGE_SIZE];

    // Read client message.
    recv(*newSocketPtr, msg, CLIENT_MESSAGE_SIZE, 0);
    
    // Receive and parse request.
    printf("Req:\n%s\n", msg);
    Request *req = Request_parse(msg);

    Response *res = Response_new(req, server->broker);
    
    // Lock thread during IO operations on the queue.
    pthread_mutex_lock(&lock);
    res->handle(res);
    pthread_mutex_unlock(&lock);
    
    res->assemble(res);

    // printf("============\nReq:\n%s\n==========\n", msg);
    printf("============\nRes:\n%s\n==========\n", res->get(res));
    if (req != NULL)
        Request_destruct(req);

    send(*newSocketPtr, res->get(res), strlen(res->get(res)), 0);
    res->destruct(res);

    // _Queue.displayAll(server->broker->queue);
    close(*newSocketPtr);

    printf("Exit socket thread.\n");
    pthread_exit(NULL);
}