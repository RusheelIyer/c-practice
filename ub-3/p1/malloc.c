#include "malloc.h"

#include <stdio.h>
#include <assert.h>

typedef struct _Block {
    /*
     * Pointer to the header of the next free block.
     * Only valid if this block is also free.
     * This is null for the last Block of the free list.
     */
    struct _Block *next;

    /*
     * Our header should always have a size of 16 Bytes.
     * This is just for 32 bit systems.
     */
    uint8_t padding[8 - sizeof(void*)];

    /*
     * The size of this block, including the header
     * Always a multiple of 16 bytes.
     */
    uint64_t size;

    /*
     * The data area of this block.
     */
    uint8_t data[];
} Block;

#define HEADER_SIZE sizeof(Block)
#define INV_HEADER_SIZE_MASK ~((uint64_t)HEADER_SIZE - 1)
#define ALLOCATED_BLOCK_MAGIC (Block*)(0xbaadf00d)

/*
 * This is the heap you should use.
 * 16 MiB heap space per default. The heap does not grow.
 */
#define HEAP_SIZE (HEADER_SIZE * 1024 * 1024)
uint8_t __attribute__ ((aligned(HEADER_SIZE))) _heapData[HEAP_SIZE];

/*
 * This should point to the first free block in memory.
 */
Block *_firstFreeBlock;

/*
 * Initializes the memory block. You don't need to change this.
 */
void initAllocator()
{
    _firstFreeBlock = (Block*)&_heapData[0];
    _firstFreeBlock->next = NULL;
    _firstFreeBlock->size = HEAP_SIZE;
}

/*
 * Gets the next block that should start after the current one.
 */
static Block *_getNextBlockBySize(const Block *current)
{
    static const Block *end = (Block*)&_heapData[HEAP_SIZE];
    Block *next = (Block*)&current->data[current->size - HEADER_SIZE];

    assert(next <= end);
    return (next == end) ? NULL : next;
}

/*
 * Dumps the allocator. You should not need to modify this.
 */
void dumpAllocator()
{
    Block *current;

    printf("All blocks:\n");
    current = (Block*)&_heapData[0];
    while (current) {
        assert((current->size & INV_HEADER_SIZE_MASK) == current->size);
        assert(current->size > 0);

        printf("  Block starting at %" PRIuPTR ", size %" PRIu64 "\n",
            ((uintptr_t)(void*)current - (uintptr_t)(void*)&_heapData[0]),
            current->size);

        current = _getNextBlockBySize(current);
    }

    printf("Current free block list:\n");
    current = _firstFreeBlock;
    while (current) {
        printf("  Free block starting at %" PRIuPTR ", size %" PRIu64 "\n",
            ((uintptr_t)(void*)current - (uintptr_t)(void*)&_heapData[0]),
            current->size);

        current = current->next;
    }
}

/*
 * Round the integer up to the block header size (16 Bytes).
 */
uint64_t roundUp(uint64_t n)
{
    (void) n;

    if (n % 16 == 0) {
        return n;
    } else { 
        uint64_t retval = n / 16;
        return (++retval * 16);
    }
}

static void *_allocate(Block **blockLink, uint64_t size) {

    Block *freeBlock = *blockLink;

    if (freeBlock->size == size) {
        *blockLink = freeBlock->next;
    } else {

        const uint64_t remainingSize = freeBlock->size - size;
        freeBlock->size = size;

        Block *newFreeBlock = _getNextBlockBySize(freeBlock);
        newFreeBlock->size  = remainingSize;
        newFreeBlock->next  = freeBlock->next;
        
        *blockLink = newFreeBlock;

    }

    // Mark the current block as allocated by setting a magic next value
    freeBlock->next = ALLOCATED_BLOCK_MAGIC;
    return &freeBlock->data[0];

}

void *my_malloc(uint64_t size)
{
    (void) size;

    uint64_t reqSize = roundUp(size) + HEADER_SIZE;
    Block *current = _firstFreeBlock;
    Block **link   = &_firstFreeBlock;

    while(current) {
        if (current->size >= reqSize)
            return _allocate(link, reqSize);

        link = &current->next;
        current = current->next;
    }

    return NULL;
}

void my_free(void *address)
{
    (void) address;

    // TODO: Implement
}


