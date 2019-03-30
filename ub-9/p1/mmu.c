#include "mmu.h"
#include <stddef.h>
#include <stdint.h>

static SegmentTable* _table;

// ----------------
// Add your global data structures here.
// ----------------
#define SEGMENT_MASK 0xE000u
#define OFFSET_MASK 0x1FFFu

typedef struct _TLBEntry {
    uint32_t virtualAddress;
    uint32_t physicalAddress;
    int counter;
} TLBEntry;

static TLBEntry TLB[TLB_SIZE];
static int count = 0;

void setSegmentTable(SegmentTable* newTable)
{
    _table = newTable;
    flushTLB();
}

int translateSegmentTable(uint32_t* address)
{
    if (address == NULL || _table == NULL)
        return -1;

    uint32_t segment = *address >> OFFSET_BITS;
    uint32_t offset = OFFSET_MASK & *address;

    if (segment >= SEGMENT_COUNT)
        return -1;

    Segment curSegment = _table->segments[segment];

    if (offset >= curSegment.length)
        return -1;

    *address = curSegment.base + offset;

    return 0;
}

void flushTLB(void)
{
    for (int i = 0; i < TLB_SIZE; i++) {
        TLBEntry entry = { 0, 0, 0 };
        TLB[i] = entry;
    }
}

void addToTLB(uint32_t virtual, uint32_t physical)
{
    int nextfree = TLB_SIZE;

    for (int i = 0; i < TLB_SIZE; i++) {
        if (TLB[i].virtualAddress == 0) {
            nextfree = i;
            break;
        }
    }

    if (nextfree != TLB_SIZE) {
        TLB[nextfree].virtualAddress = virtual;
        TLB[nextfree].physicalAddress = physical;
        TLB[nextfree].counter = count++;
        return;
    }

    nextfree = 0;
    for (int i = 0; i < TLB_SIZE; i++) {
        if (TLB[i].counter < TLB[nextfree].counter) {
            nextfree = i;
        }
    }

    TLB[nextfree].virtualAddress = virtual;
    TLB[nextfree].physicalAddress = physical;
    TLB[nextfree].counter = count++;
}

int translateTLB(uint32_t* address)
{
    for (int i = 0; i < TLB_SIZE; i++) {
        if (TLB[i].virtualAddress == *address) {
            return TLB[i].physicalAddress;
        }
    }

    return -1;
}
