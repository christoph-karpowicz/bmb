#include "mem.h"

int mem_allocated = 0;
int mem_freed = 0;

void *mem_alloc(size_t size)
{
    void *p = malloc(size);
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed, %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    mem_allocated++;
    return p;
}

void mem_free(void *p)
{
    if (p != NULL) {
        free(p);
        mem_freed++;
        p = NULL;
    }
}
