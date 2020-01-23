#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "queue/queue.h"
#include "queue/queue_node.h"
#include "queue/queue_pool.h"
#include "server/server.h"
#include "server/s_thread.h"
#include "util/util.h"
#include "util/mem.h"

/**
 * app_init - cleanup before closing the app.
 */
__attribute__((destructor))
void app_close()
{
    if (server_ptr == NULL) return;
    queue_pool_destruct(server_ptr->broker->queuePool);
    printf("* request count: %d\n", server_ptr->requestCounter);
    Server_destruct(server_ptr);
    printf("* memory allocations: %d\n", mem_allocated);
    printf("* memory freed: %d\n", mem_freed);
    printf("========\n");
}

/**
 * close_signal_handler - function called after the 
 * application receives a SIGINT signal.
 * @sig: signal
 */
void close_signal_handler(int sig)
{
    char c;

    printf("\n========\n");
    printf("Ending ...\n");

    signal(sig, SIG_IGN);
    printf("Do you really want to quit? [y/n]");

    c = getchar();
    if (c == 'y' || c == 'Y') {
        exit(1);
    }
    else {
        signal(SIGINT, close_signal_handler);
    }
}

/**
 * app_init - initializes the application, starts the socket server. 
 */
__attribute__((constructor))
void app_init()
{
    signal(SIGINT, close_signal_handler);

    Server server;
    Server_init(&server);
    Server_create_socket(&server);
    Server_bind(&server);

    server_ptr = &server;

    // Listen on the socket, with 50 max connection requests queued.
    if (listen(server.socket, 50) == 0) {
        log("Listening\n");
    }
    else
        server.error("ERROR on listen attempt");
}

int main() {
    pthread_t tid[50];
    int i = 0;

    while(1) {
        socket_thread_args args; 

        Server_accept(server_ptr);

        args.server = server_ptr;
        args.time_start = get_epoch_milis();

        // Create separate thread for received client request.
        if (pthread_create(&tid[i], NULL, socketThread, (void *)&args) != 0)
            server_ptr->error("ERROR Failed to create thread");

        log("Request count: %d\n", server_ptr->requestCounter);

        if (i >= 50) {
            i = 0;
            while(i < 50) {
                pthread_join(tid[i++], NULL);
            }
            i = 0;
        }
    }
        
    return 0;
    
}