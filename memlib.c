/* memlib.c is the implementation of 
 *  the memory operations declared in memlib.h
 */

#include "memlib.h"

/*-----------------------------------*
 *   Global Variables initialization
 *-----------------------------------*/
char *mem_heap;             /* points to the last byte of the simulated heap */
char *mem_brk;              /* points to last byte of the simulated heap */

/* mem_init -- initializes the heap space */
void mem_init(void)
{   
    /* mem_brk points to the end of data segment when heap space is empty, same as mem_heap */
    mem_heap = (char *)sbrk(0);
    mem_brk = mem_heap;
}

/* mem_sbrk -- increases the heap space by inrc bytes */
void *mem_sbrk(size_t inrc)
{   /* Moves mem_brk pointer by inrc bytes, which means increasing the heap space by inrc bytes */

    char *old_brk = mem_brk;
    /* Pay attention that the heap space cannot be shrunk here */
    if(inrc < 0)
    {
        fprintf(stderr, "ERROR: mem_sbrk() failed, inrc cannot be negative!");
        return NULL;
    }

    /* Run out of Memory */
    mem_brk = sbrk(inrc);
    if(inrc < 0 || mem_brk == (void *)(-1))
    {
       //errno = ENONEM;
       fprintf(stderr, "ERROR: mem_sbrk() failed, Run out of Memory!");
    }

    /* heap space created successfully */
    mem_brk += inrc;
    return (void *)old_brk;
}

/* mem_deinit -- frees the storage of memory allocated to the simulated heap model */
void mem_deinit(void){}

/* mem_reset_brk -- resets the heap tp empty */
void mem_reset_brk(void) 
{
    mem_brk = mem_heap;
}

/* mem_low -- returns the first byte address of the simulated heap space */
void *mem_low(void)
{
    return (void *)mem_heap;
}

/* mem_high -- returns the last byte address of the simulated heap space */
void *mem_high(void)
{
    return (void *)(mem_brk - 1);
}

/* mem_page_size -- returns the system page size */
size_t mem_page_size(void){
    return (size_t)getpagesize();
}

/* mem_heap_size -- returns the current simulated heap space's size */
size_t mem_heap_size(void)
{
    return (size_t)(mem_brk - mem_heap);
}