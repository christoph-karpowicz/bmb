#ifndef MEM_H
#define MEM_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include <string.h>

extern int mem_allocated;
extern int mem_freed;
extern int errno;

void *mem_alloc(size_t size);

void mem_free(void *p);

#endif