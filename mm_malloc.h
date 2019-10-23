/* mm_malloc.h declares functions malloc() and free() */
#ifndef MM_MALLOC_H
#define MM_MALLOC_H
#include <unistd.h>

/* mm_init -- Initializeds the simulated heap model */
int mm_init(void);

/* mm_malloc -- Allocates heap space with at least size payload */
void *mm_malloc(size_t size);

/* mm_free -- Frees the allocated memory storage */
void mm_free(void *ptr);
#endif