#include "mmu.h"
#include <string.h> // only for memset.
#include <assert.h>

static SegmentTable *_table = NULL;

typedef struct {
    uint32_t virtual;
    uint32_t physical;
    // Last access time.
    uint32_t lastAccess;
    int valid;
} TLBEntry;

// Counter for the LRU strategy. We assume that this will never overflow.
static uint32_t _accessCounter = 0;
static TLBEntry _tlb[TLB_SIZE];

// All bits except the first three bits.
// Since the segment has 3 bits, this is 0x1ffffff
#define OFFSET_MASK ((uint32_t) (1UL << OFFSET_BITS) - 1)

void setSegmentTable(SegmentTable *newTable)
{
    _table = newTable;
    flushTLB();
}

int translateSegmentTable(uint32_t *address)
{
    if ((_table == NULL) || (address == NULL)) {
        return -1;
    }

    // The index of the segment is given by the first 3 bits.
    // We shift the address to the right so that only those bits are left over.
    uint32_t segmentIndex = *address >> OFFSET_BITS;
    assert(segmentIndex <= SEGMENT_COUNT);

    // The index inside the segment is given by the rest of the bits.
    // We mask off the 3 most significant bits.
    uint32_t offset = *address & OFFSET_MASK;

    Segment *segment = &_table->segments[segmentIndex];
    if (offset < segment->length) {
        // The address is only valid if the offset is inside the segment.
        // Now we compute the physical address. Note that the base + offset
        // method with segmentation is different from the concatenation method
        // used in paging.
        *address = segment->base + offset;
        return 0;
    }

    return -1;
}

void flushTLB(void)
{
    memset(_tlb, 0, sizeof(_tlb));
}

void addToTLB(uint32_t virtual, uint32_t physical)
{
    // Find the oldest entry that is currently in the TLB.
    // Invalid entries have an access time of 0, but are preferred.
    int oldestEntry = 0;
    for (int i = 1; i < TLB_SIZE; ++i) {
        if (!_tlb[i].valid || (_tlb[i].lastAccess < _tlb[oldestEntry].lastAccess)) {
            oldestEntry = i;
        }
    }

    // Set that entry in the TLB.
    _tlb[oldestEntry].valid    = 1;
    _tlb[oldestEntry].physical = physical;
    _tlb[oldestEntry].virtual  = virtual;

    // Remember to set and increment the access counter, since this also
    // counts as use of the entry.
    _tlb[oldestEntry].lastAccess = _accessCounter;
    _accessCounter++;
}

int translateTLB(uint32_t *address)
{
    if ((_table == NULL) || (address == NULL)) {
        return -1;
    }

    // We have a fully associative TLB. That means that we need to check
    // every entry of the TLB.
    for (int i = 0; i < TLB_SIZE; i++) {
        if (_tlb[i].valid && (_tlb[i].virtual == *address)) {
            // We found a TLB entry for this virtual address and use it.
            *address = _tlb[i].physical;

            _tlb[i].lastAccess = _accessCounter;
            _accessCounter++;
            return 0;
        }
    }

    return -1;
}