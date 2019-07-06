#ifndef S_THREAD_H
#define S_THREAD_H

#include <pthread.h>

extern pthread_mutex_t lock;

void* socketThread(void *args);

#endif