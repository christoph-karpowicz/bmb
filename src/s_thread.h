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
#include "queue.h"
#include "queue_node.h"
#include "request.h"
#include "response.h"

extern pthread_mutex_t lock;

void* socketThread(void *args);

#endif