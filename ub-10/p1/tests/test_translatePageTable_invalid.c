#include "testlib.h"
#include "page_table.h"

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main() {
	test_start("Your translatePageTable fails for invalid virtual addresses");

	basePageDirectory.entries[1] = pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

	pageTable1.entries[1] =0x2000 | PAGE_PRESENT_MASK;
	pageTable1.entries[3] =0x3000 | PAGE_PRESENT_MASK | PAGE_READWRITE_MASK;
	pageTable1.entries[4] =0x4000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
	pageTable1.entries[5] =0x5000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK | PAGE_READWRITE_MASK;
	setPageDirectory(&basePageDirectory);

	uint32_t address = (1 * ENTRIES_PER_TABLE + 2) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_READ, KERNEL_MODE), -1, "translatePageTable fails");
	test_equals_int64(address, (1 * ENTRIES_PER_TABLE + 2) * (1<<OFFSET_BITS), "address is unchanged");
	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 7) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_READ, KERNEL_MODE), -1, "translatePageTable fails");
	test_equals_int64(address, (1 * ENTRIES_PER_TABLE + 7) * (1<<OFFSET_BITS), "address is unchanged");

	return test_end();
}

