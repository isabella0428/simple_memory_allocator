#include "mm_malloc.h"
#include "memlib.h"
/* ------------------------------------------------- */
/*                   Constants                       */
/* ------------------------------------------------- */
#define WSIZE 4             /* Word and header/footer suze (bytes) */
#define DSIZE 8             /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12) /* Extend heap by this amount (bytes) */

/*-------------------------------------
                Macros
--------------------------------------*/
#define MAX(x, y) ((x) > (y)? (x) : (y))

/*Pack size with allocated bit into a word */
#define PACK(size, alloc)   ((size) | (alloc))

/* Get the value the pointer points to*/
#define GET(bp)     (*(unsigned int *)(bp))
/* Put the value into the address */
#define PUT(p, val)     (*(unsigned int *)(p) = val)

/* Get block size from header/footer */
#define GET_SIZE(bp)    (GET(bp) & (~0x7))
/* Get allocated info from header/footer */
#define GET_ALLOC(bp)   (GET(bp) & (0x1))

/* Return header of the block bp points to */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
/* Return footer of the block bp points to */
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Return next block's pointer */
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(HDRP(bp)))
/* Return previous block's pointer */
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

/* ------------------------------------------------- */
/*              Global variables                     */
/* ------------------------------------------------- */
static char *heap_pointer = 0;        /* Pointer to first block */

/* ------------------------------------------------- */
/* Function prototypes for internal helper routines  */
/* ------------------------------------------------- */

/* extend_heap - Extend heap with free block and return its block pointer */
static void *extend_heap(size_t words);

/* coalsce - Boundary tag coalscing. Return pointer to coalesced block */
static void *coalesce(void *bp);

/* place - place block of asize bytes at start of free block bp and split it */
static void place(void *bp, size_t asize);

/* find_fit - Find a fit for a block with asize bytes */
static void *find_fit(size_t asize);

/* print_block - Print the information of bp */
static void print_block(void *bp);

/* check_block - Minimal check of the block */
static void check_block(void *bp);

void check_heap(int verbose);

/* ------------------------------------------------- */
/* Implenmentation of functions declared in mm.h     */
/* ------------------------------------------------- */
/* mm_init - Initialize the memory manager */
int mm_init(void)
{
    mem_init();
    heap_pointer = mem_sbrk(4 * WSIZE);
    if(heap_pointer == (void *)(-1))
        return -1;

    PUT(heap_pointer, 0);                             /* Alignment padding */
    PUT(heap_pointer + WSIZE, PACK(DSIZE, 1));      /* Header */
    PUT(heap_pointer + (2*WSIZE), PACK(DSIZE, 1));    /* Prologue header */
    PUT(heap_pointer + (3*WSIZE), PACK(0, 1));        /* Epilogue header */
    heap_pointer += 2 * WSIZE;

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if(!extend_heap(CHUNKSIZE / WSIZE))
        return -1;
    return 0;
}

/* mm_malloc - Allocate a block with at least size bytes of payload */
void *mm_malloc(size_t size)
{
    size_t asize;               /* Adjusted block size */
    size_t extendSize;          /* Amount to extend heap if no fit */
    char *bp;                   /* Block pointer */
    if(heap_pointer == NULL) /* Make sure that memory structure exists */
        mm_init();

    if(size == 0)
        return NULL;        /* Size of the memory space to be allocated must be positive */


    /* Rounded to multiple of 8 bytes */
    if(size < DSIZE){
        asize = DSIZE;
    } else{
        asize = ((size + 2 * DSIZE - 1) / DSIZE) * DSIZE;
    }

    bp = find_fit(asize);
    if(bp != NULL){
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendSize = MAX(asize, CHUNKSIZE);
    bp = extend_heap(extendSize);
    if(bp != NULL)         /* Extends the heap space if current blocks are not big enough */
        return NULL;
    place(bp, asize);
    return bp;

}

/* mm_free -- Frees the memory storage allocated to the memory structure */
void mm_free(void * bp)
{
    if(bp == NULL)
        return;

    size_t size =  GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));      /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));      /* Free block footer */

    coalesce(bp);                       /* Coalesces the free blocks */
}

/* extend_heap -- Extends the heap space with at least size words */
static void *extend_heap(size_t words)
{
    size_t byte_size;      /* Size to be extended in bytes */
    char *bp;           /* Start address of the extended heap */

    if(!words)
        return NULL;

    if(words % 2 == 1)          /* Extended size should be multiples of words */
        words += 1;
    byte_size = WSIZE * words;
    //printf("Extend Byte size %d\n", (int)byte_size);

    bp =  mem_sbrk(byte_size);
    if(bp == (void *)(-1))
        return NULL;

    PUT(HDRP(bp), PACK(byte_size, 0));      /* Header of the free block */
    PUT(FTRP(bp), PACK(byte_size, 0));      /* Footer of the free block */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));         /* Header of the nex block */

    return coalesce(bp);
}


/* coalesce -- Concatenate the neignbouring free blocks */
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc)
    {   /* Both the previous block and the following block are allocated. */
        return bp;
    }else if(prev_alloc && !next_alloc)
    {
        /* ptr needs to coalesce with the next block */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));         //TODO:whether it is the next footer?
    }else if(!prev_alloc && next_alloc)
    {
        /* ptr needs to coalesce with the previous block */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(bp), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }else{
        /* Both previous block and the next block should be coalesced with current block */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return coalesce(bp);  //TODO: no digui?
}


static void place(void *bp, size_t alloc_size)
{
    size_t block_size = GET_SIZE(HDRP(bp));
    if(block_size - alloc_size < 2 * DSIZE)
    {
        /* Internal Fragmentation is not enough to become a new unit */
        PUT(HDRP(bp), PACK(block_size, 1));
        PUT(FTRP(bp), PACK(block_size, 1));

    } else
    {
        /* Internal Fragmentation is big enough to become a new memory block */
        PUT(HDRP(bp), PACK(alloc_size, 1));
        PUT(FTRP(bp), PACK(alloc_size, 1));

        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(block_size - alloc_size, 0));
        PUT(FTRP(bp), PACK(block_size - alloc_size, 0));
    }
}

static void *find_fit(size_t size)
{
    void *bp;   /* The block pointer of the suitable memory block */

    for(bp = heap_pointer; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        /* Find memory free and big enough memory blocks */
        if(!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= size)
        {
            return bp;
        }
    }
    return NULL;    /* Return NULL if not find anything */
}

/* print_block -- Print the information of the memory block */
static void print_block(void *bp)
{
    size_t fsize, hsize, falloc,halloc;
    fsize = GET_SIZE(FTRP(bp));
    hsize = GET_SIZE(HDRP(bp));
    falloc = GET_ALLOC(FTRP(bp));
    halloc = GET_ALLOC(HDRP(bp));

    if(hsize == 0)
    {
        printf("%p, EOL\n", bp);
        return;
    }

    printf("%p: header: [%d:%c] footer: [%d:%c]\n", bp,
           (int)hsize, (halloc ? 'a':'f'),
           (int)fsize, (falloc ? 'a':'f'));
}

/* check_heap - Minimal check of the heap for consistency */
void check_heap(int verbose)
{
    char *bp = heap_pointer;
    if(verbose)
        printf("Heap (%p):\n", heap_pointer);

    if(GET_SIZE(HDRP(heap_pointer)) != DSIZE || !GET_ALLOC(HDRP(heap_pointer)))
        printf("Bad prologue header\n");

    check_block(heap_pointer);

    for(bp = heap_pointer; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if(verbose)
            print_block(bp);
        check_block(bp);
    }

    if(verbose)
        print_block(bp);
    if((GET_SIZE(HDRP(bp)) != 0) || !GET_ALLOC(HDRP(bp)))
        printf("Bad epilogue header\n");
}

/* check_block -- Check the block status */
void check_block(void *bp)
{
    if((size_t)(bp) % 8)
        printf("Error: %p is not double word aligned!", bp);
    if(GET_SIZE(HDRP(bp)) != GET_SIZE(FTRP(bp)) || GET_ALLOC(HDRP(bp)) != GET_ALLOC(FTRP(bp)))
        printf("Error: header does not match footer!");
}
