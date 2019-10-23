/* memlib.h declares many memory functions */

#ifndef MEMLIB_H
#define MEMLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

/* Initialzes the simulated memory model */
void mem_init(void);

/** Implements an easy version of Unix call sbrk
 *  Move the simulated program break pointer by incr bytes and return the old pointer address.
 *  Pay attention that the memory area cannot be shrunk here.
 */  
void *mem_sbrk(size_t incr);

/* Frees the storage allocated to the memory model */
void mem_deinit(void);

/* Resets the simulated program break pointer to an empty heap */
void mem_reset_brk(void);

/* Returns address of the heap's first byte*/
void *mem_heap_low(void);

/* Returns address of the heap's last byte*/
void *mem_heap_high(void);

/* Returns the page size of the system */
size_t mem_page_size();

/* Returns the heap size in bytes */
size_t mem_heap_size();
#endif