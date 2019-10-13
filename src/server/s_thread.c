#include "s_thread.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *socketThread(void *args)
{

    printf("Socket thread opened.\n");
    socket_thread_args *arguments = (socket_thread_args*)args;

    int *newSocketPtr = arguments->socket;
    int newSocket = *newSocketPtr;

    char *client_msg;
    char *buff;

    client_msg = arguments->client_message;
    buff = arguments->buffer;

    Queue *queue = arguments->queue;

    // Read client message.
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    recv(newSocket, client_msg, CLIENT_MESSAGE_SIZE, 0);
    
    // Receive and parse request.
    // printf("Req:\n%s\n", client_msg);
    Request *req = Request_parse(client_msg);    
    // printf("****req %s\n", req->message);

    Response *res = Response_new(req, queue);
    
    // Lock thread during IO operatins on th queue.
    pthread_mutex_lock(&lock);
    res->handle(res);
    pthread_mutex_unlock(&lock);
    res->assemble(res);

    Request_destruct(req);

    // printf("Res:\n%s\n", res->get(res));
    // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    printf("Time:%llu\n", getEpochMilis() - arguments->time_start);
    send(newSocket, res->get(res), strlen(res->get(res)), 0);
    res->destruct(res);

    _Queue.displayAll(queue); 
    printf("Exit socket thread.\n");
    close(newSocket);

    pthread_exit(NULL);
    
}