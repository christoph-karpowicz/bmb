#include "mem.h"

// Memory allocation and freeing counters for debugging.
int mem_allocated = 0;
int mem_freed = 0;

/**
 * mem_alloc - custom replacement for malloc. 
 * @size: allocated memory size
 */
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

/**
 * mem_free - custom replacement for free.
 * @p: pointer to memory address being freed
 */
void mem_free(void *p)
{
    if (p != NULL) {
        free(p);
        mem_freed++;
        p = NULL;
    }
}
