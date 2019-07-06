#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define CLIENT_MESSAGE_SIZE 2000

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct socket_thread_arguments {
    int* socket;
    char* client_message;
    char* buffer;
} socket_thread_args;

void* socketThread(void *args)
{
    socket_thread_args* arguments = (socket_thread_args*)args;

    int* newSocketPtr = arguments->socket;
    int newSocket = *newSocketPtr;

    char* client_msg;
    char* buff;

    client_msg = arguments->client_message;
    buff = arguments->buffer;

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
    free(message);

    pthread_mutex_unlock(&lock);

    // sleep(1);
    // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    send(newSocket, buff, BUFFER_SIZE, 0);
    printf("Exit socketThread \n");
    close(newSocket);

    pthread_exit(NULL);
    
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main() {

    char client_message[CLIENT_MESSAGE_SIZE] = "\0";
    char buffer[BUFFER_SIZE] = "\0";

    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    //Create the socket. 
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) 
        error("ERROR opening socket");

    // Configure settings of the server address struct
    // Address family = Internet 
    serverAddr.sin_family = AF_INET;

    //Set port number, using htons function to use proper byte order 
    serverAddr.sin_port = htons(PORT);

    //Set IP address to localhost 
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Set all bits of the padding field to 0 
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Bind the address struct to the socket 
    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
        error("ERROR on binding");

    //Listen on the socket, with 40 max connection requests queued 
    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        error("ERROR on listen attempt");

    pthread_t tid[60];
    int i = 0;
    while(1)
    {

        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        if (newSocket < 0) 
            error("ERROR on accept");

        socket_thread_args args;
        args.socket = &newSocket;
        args.client_message = client_message;
        args.buffer = buffer;

        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
        if (pthread_create(&tid[i], NULL, socketThread, (void*)&args) != 0)
            error("ERROR Failed to create thread");

        if (i >= 50)
        {
            i = 0;
            while(i < 50)
            {
                pthread_join(tid[i++],NULL);
            }
            i = 0;
        }
    }
        
    return 0;
    
}