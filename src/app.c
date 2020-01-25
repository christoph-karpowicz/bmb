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
 * app_close - cleanup before closing the app.
 */
__attribute__((destructor))
void app_close()
{
    if (server == NULL) return;
    queue_pool_destruct(server->broker->queuePool);
    printf("* request count: %d\n", server->requestCounter);
    Server_destruct(server);
    printf("* memory allocations: %d\n", mem_allocated);
    printf("* memory freed: %d\n", mem_freed);
    printf("* (two weren't freed because the last thread hasn't been opened)\n");
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

    server = (Server *) mem_alloc(sizeof(Server));
    Server_init(server);
    Server_create_socket(server);
    Server_bind(server);

    // Listen on the socket, with 50 max connection requests queued.
    if (listen(server->socket, 50) == 0) {
        log("Listening on port %d.\n", PORT);
    }
    else
        server->error("Error on listen attempt.");
}

int main() {
    pthread_t tid;
    size_t i = 0;

    while(1) {
        socket_thread_args *args = (socket_thread_args *) mem_alloc(sizeof(socket_thread_args));
        Connection *conn = (Connection *) mem_alloc(sizeof(Connection));

        conn->socket_fd = Server_accept(server, conn);

        args->conn       = conn;
        args->server     = server;
        args->time_start = get_epoch_milis();

        // Create separate thread for received client request.
        if (pthread_create(&tid, NULL, broker_thread, (void *)args) != 0)
            server->error("Error: failed to create thread.");

        log("Request count: %d\n", server->requestCounter);
    }
        
    return 0;
}