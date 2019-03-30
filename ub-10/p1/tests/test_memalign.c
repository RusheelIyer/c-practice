#include "testlib.h"
#include "page_table.h"
#include <stdlib.h>

int __real_posix_memalign(void **memptr, size_t alignment, size_t size);
int __wrap_posix_memalign(void **memptr, size_t alignment, size_t size) {
	test_equals_int64(size, 4096, "posix_memalign: size is correct");
	test_equals_int64(alignment, 4096, "posix_memalign: alignment is correct");
	return __real_posix_memalign(memptr, alignment, size);
}

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main() {
	test_start("Your mapPage uses posix_memalign with the right parameters to allocate a new page table.");
	test_plan(5);

	basePageDirectory.entries[1] = pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

	uint32_t entry2 =  0x1000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
	uint32_t entry4 =  0x2000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
	pageTable1.entries[2] =entry2;
	pageTable1.entries[4] =entry4;
	setPageDirectory(&basePageDirectory);

	test_equals_int(mapPage((2 * ENTRIES_PER_TABLE + 3) * (1<<OFFSET_BITS), 0x3000, ACCESS_READ, KERNEL_MODE), 0, "mapPage succeeds");

	test_equals_int64(basePageDirectory.entries[2] & PAGE_PRESENT_MASK, PAGE_PRESENT_MASK, "entry 2: page is present");
	PageTable *table = (PageTable*) intToPointer(basePageDirectory.entries[2] & PAGE_DIRECTORY_ADDRESS_MASK);
	int success = 1;
	for (int i = 20; i < ENTRIES_PER_TABLE; i++) {
		if ((table->entries[i] & PAGE_PRESENT_MASK) != 0) success = 0;
	}
	test_assert(success, "other pages are not present");

	return test_end();
}

