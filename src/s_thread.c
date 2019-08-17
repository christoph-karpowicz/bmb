#include "s_thread.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* socketThread(void *args)
{

    socket_thread_args* arguments = (socket_thread_args*)args;

    int* newSocketPtr = arguments->socket;
    int newSocket = *newSocketPtr;

    char* client_msg;
    char* buff;

    client_msg = arguments->client_message;
    buff = arguments->buffer;

    Queue *queue = arguments->queue;

    // Read client message.
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    recv(newSocket, client_msg, CLIENT_MESSAGE_SIZE, 0);
    
    pthread_mutex_lock(&lock);
    
    // Send message to the client socket.
    char *message = malloc(CLIENT_MESSAGE_SIZE + 20);
    strcpy(message, "12345678901234567890\n");
    strcat(message, client_msg);
    strcat(message, "\n");
    strcpy(buff, message);

    // printf("test1 -- %ld\n", strlen(client_msg));

    // Receive and parse request.
    Request *request = Request_parse(client_msg);    
    printf("****req %s\n", request->message);

    Node *newNode = Node_new();
    _Node.setMessage(newNode, request->message, strlen(request->message));
    _Queue.add(queue, newNode);

    Request_destruct(request);

    free(message);

    pthread_mutex_unlock(&lock);

    // sleep(1);
    // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    send(newSocket, buff, BUFFER_SIZE, 0);
    printf("Exit socketThread \n");
    close(newSocket);

    pthread_exit(NULL);
    
}
