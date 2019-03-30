#include "mmu.h"
#include <stddef.h>
#include <stdint.h>

static SegmentTable* _table;

// ----------------
// Add your global data structures here.
// ----------------
#define SEGMENT_MASK 0xE000u
#define OFFSET_MASK 0x1FFFu

void setSegmentTable(SegmentTable* newTable)
{
    _table = newTable;
    flushTLB();
}

int translateSegmentTable(uint32_t* address)
{
    if (address == NULL || _table == NULL)
        return -1;

    uint32_t segment = (SEGMENT_MASK & *address) >> OFFSET_BITS;
    uint32_t offset = OFFSET_MASK & *address;

    if (segment >= SEGMENT_COUNT)
        return -1;

    Segment curSegment = _table->segments[segment];

    if (offset > curSegment.length)
        return -1;

    *address = curSegment.base + offset;

    return 0;
}

void flushTLB(void)
{
}

void addToTLB(uint32_t virtual, uint32_t physical)
{
    (void)virtual;
    (void)physical;
}

int translateTLB(uint32_t* address)
{
    (void)address;
    return -1;
}
