#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "server.h"
#include "s_thread.h"
#include "queue.h"
#include "queue_node.h"

int main() {

    Server server;
    Server_init(&server);
    Server_create_socket(&server);
    Server_bind(&server);

    //Listen on the socket, with 50 max connection requests queued 
    if (listen(server.socket, 50) == 0)
        printf("Listening\n");
    else
        error("ERROR on listen attempt");

    pthread_t tid[50];
    int i = 0;
    
    while(1)
    {
 
        Server_accept(&server);

        socket_thread_args args; 
        args.socket = &server.newSocket;
        args.client_message = server.client_message;
        args.buffer = server.buffer;

        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
        if (pthread_create(&tid[i], NULL, socketThread, (void *)&args) != 0)
            error("ERROR Failed to create thread");

        if (i >= 50)
        {
            i = 0;
            while(i < 50)
            {
                pthread_join(tid[i++], NULL);
            }
            i = 0;
        }
        
    }
        
    return 0;
    
}