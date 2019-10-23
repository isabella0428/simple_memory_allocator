/**
* This file is intended to test my mm_malloc() and mm_free function
*/
#include "mm_malloc.h"
#include "memlib.h"

#define MAX 40

int main(int argc, char* argv[])
{
    void* m_start = sbrk(0);
    size_t malloc_size = 0;

    for(size_t i = 0; i < MAX; i += 2)
    {
        void *ptr_f = mm_malloc(i);
        void *ptr = mm_malloc(i + 1);
        mm_free(ptr_f);
        malloc_size += i + 1;
    }

    void *m_end = sbrk(0);
    size_t heap_size = (size_t)(m_end - m_start);

    printf("malloc size: %ld, heap size: %ld\n", malloc_size, heap_size);
    check_heap(1);
    return 0;
}