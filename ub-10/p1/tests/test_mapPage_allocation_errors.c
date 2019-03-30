#include "testlib.h"
#include "page_table.h"
#include <stdlib.h>

int posix_memalign(void **memptr, size_t alignment, size_t size) {
	(void) memptr;
	(void) alignment;
	(void) size;
	test_assert(1, "You call posix_memalign");
	return -1;
}

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main() {
	test_start("Your mapPage handles allocation errors.");
	test_plan(5);

	basePageDirectory.entries[1] =
			pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

	uint32_t entry2 = 0x1000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
	uint32_t entry4 = 0x2000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
	pageTable1.entries[2] = entry2;
	pageTable1.entries[4] = entry4;
	setPageDirectory(&basePageDirectory);
	uint64_t oldDir = basePageDirectory.entries[1];
	uint64_t oldDir2 = basePageDirectory.entries[2];

	test_equals_int(
			mapPage((2 * ENTRIES_PER_TABLE + 3) * (1<<OFFSET_BITS), 0x3000, ACCESS_READ, KERNEL_MODE),
			-1, "mapPage fails");

	test_equals_int64(basePageDirectory.entries[2] & PAGE_PRESENT_MASK, 0, "page directory entry 2 is not present");
	test_equals_int64(basePageDirectory.entries[1], oldDir, "page directory entry 1 is unchaged");
	test_equals_int64(basePageDirectory.entries[2], oldDir2, "page directory entry 2 is unchanged");

	return test_end();
}

