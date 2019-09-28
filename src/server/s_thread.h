#ifndef S_THREAD_H
#define S_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "server.h"
#include "../queue/queue.h"
#include "../queue/queue_node.h"
#include "../http/request.h"
#include "../http/response.h"

extern pthread_mutex_t lock;

void* socketThread(void *args);

#endif