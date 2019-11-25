#include "s_thread.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *socketThread(void *args)
{
    printf("Socket thread opened.\n");

    socket_thread_args *arguments = (socket_thread_args*)args;
    Server *server =                arguments->server;
    Queue *queue =                  server->queue;
    int *newSocketPtr =             &server->newSocket;
    char                            msg[CLIENT_MESSAGE_SIZE];

    // Read client message.
    recv(*newSocketPtr, msg, CLIENT_MESSAGE_SIZE, 0);
    
    // Receive and parse request.
    // printf("Req:\n%s\n", msg);
    Request *req = Request_parse(msg);

    Response *res = Response_new(req, queue);
    
    // Lock thread during IO operatins on th queue.
    pthread_mutex_lock(&lock);
    res->handle(res);
    pthread_mutex_unlock(&lock);
    
    res->assemble(res);

    // if (req == NULL) {
        // printf("============\nReq:\n%s\n==========\n", msg);
        printf("============\nRes:\n%s\n==========\n", res->get(res));
    // }
    if (req != NULL)
        Request_destruct(req);

    send(*newSocketPtr, res->get(res), strlen(res->get(res)), 0);
    res->destruct(res);

    _Queue.displayAll(queue);
    close(*newSocketPtr);

    printf("Exit socket thread.\n");
    pthread_exit(NULL);
    
}