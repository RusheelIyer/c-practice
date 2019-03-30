#ifndef MMU_H
#define MMU_H

#include <inttypes.h>

//    3 bits                     29 bits
// +---------+------------------------------------------------+
// | Segment |                    Offset                      |
// +---------+------------------------------------------------+
#define SEGMENT_BITS 3
#define OFFSET_BITS (32 - SEGMENT_BITS)

// Number of segments (hardcoded)
#define SEGMENT_COUNT (1UL << SEGMENT_BITS)

#define TLB_SIZE 4

typedef struct {
    // Physical base address.
    uint32_t base;
    // Physical length of the segment (in bytes).
    uint32_t length;
} Segment;

typedef struct {
    Segment segments[SEGMENT_COUNT];
} SegmentTable;

void setSegmentTable(SegmentTable *segments);

// Translate a virtual address to the physical address.
int translateSegmentTable(uint32_t *address);

void flushTLB(void);
void addToTLB(uint32_t physical, uint32_t virtual);

// Look up a physical address using the MMU.
int translateTLB(uint32_t *address);

#endif