#include "testlib.h"
#include "page_table.h"

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main() {
	test_start("Your mapPage updates an existing entry.");

	basePageDirectory.entries[1] = pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

	uint32_t entry2 =  0x1000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
	uint32_t entry4 =  0x2000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
	pageTable1.entries[2] =entry2;
	pageTable1.entries[4] =entry4;
	setPageDirectory(&basePageDirectory);

	test_equals_int(mapPage((1 * ENTRIES_PER_TABLE + 2) * (1<<OFFSET_BITS), 0x3000, ACCESS_READ, KERNEL_MODE), 0, "mapPage of entry 2 succeeds");

	test_equals_int64(pageTable1.entries[2] & PAGE_TABLE_ADDRESS_MASK, 0x3000, "entry 2: address is correct");
	test_equals_int64(pageTable1.entries[2] & PAGE_PRESENT_MASK, PAGE_PRESENT_MASK, "entry 2: page is present");
	test_equals_int64(pageTable1.entries[4], entry4, "entry 4 is unchanged");

	return test_end();
}

