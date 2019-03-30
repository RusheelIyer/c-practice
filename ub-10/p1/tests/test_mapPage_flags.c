#include "testlib.h"
#include "page_table.h"

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main() {
	test_start("Your mapPage sets the right flags.");

	basePageDirectory.entries[1] = pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;
	pageTable1.entries[10] = 0x1000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK | PAGE_ACCESSED_MASK;
	setPageDirectory(&basePageDirectory);

	test_equals_int(mapPage((1 * ENTRIES_PER_TABLE + 10) * (1<<OFFSET_BITS), 0x3000, ACCESS_READ, KERNEL_MODE), 0, "mapPage of entry 10 succeeds");
	test_equals_int(mapPage((1 * ENTRIES_PER_TABLE + 11) * (1<<OFFSET_BITS), 0x3000, ACCESS_WRITE, KERNEL_MODE), 0, "mapPage of entry 11 succeeds");
	test_equals_int(mapPage((1 * ENTRIES_PER_TABLE + 12) * (1<<OFFSET_BITS), 0x3000, ACCESS_READ, USER_MODE), 0, "mapPage of entry 12 succeeds");
	test_equals_int(mapPage((1 * ENTRIES_PER_TABLE + 13) * (1<<OFFSET_BITS), 0x3000, ACCESS_WRITE, USER_MODE), 0, "mapPage of entry 13 succeeds");

	test_equals_int64(pageTable1.entries[10] & PAGE_PRESENT_MASK, PAGE_PRESENT_MASK, "entry 10: page is present");
	test_equals_int64(pageTable1.entries[10] & PAGE_READWRITE_MASK, 0, "entry 10: page is not writable");
	test_equals_int64(pageTable1.entries[10] & PAGE_USERMODE_MASK, 0, "entry 10: page is kernel mode");
	test_equals_int64(pageTable1.entries[10] & PAGE_ACCESSED_MASK, 0, "entry 10: page has not been accessed");

	test_equals_int64(pageTable1.entries[11] & PAGE_PRESENT_MASK, PAGE_PRESENT_MASK, "entry 11: page is present");
	test_equals_int64(pageTable1.entries[11] & PAGE_READWRITE_MASK, PAGE_READWRITE_MASK, "entry 11: page is writable");
	test_equals_int64(pageTable1.entries[11] & PAGE_USERMODE_MASK, 0, "entry 11: page is kernel mode");
	test_equals_int64(pageTable1.entries[11] & PAGE_ACCESSED_MASK, 0, "entry 11: page has not been accessed");

	test_equals_int64(pageTable1.entries[12] & PAGE_PRESENT_MASK, PAGE_PRESENT_MASK, "entry 12: page is present");
	test_equals_int64(pageTable1.entries[12] & PAGE_READWRITE_MASK, 0, "entry 12: page is not writable");
	test_equals_int64(pageTable1.entries[12] & PAGE_USERMODE_MASK, PAGE_USERMODE_MASK, "entry 12: page is user mode");
	test_equals_int64(pageTable1.entries[12] & PAGE_ACCESSED_MASK, 0, "entry 12: page has not been accessed");

	test_equals_int64(pageTable1.entries[13] & PAGE_PRESENT_MASK, PAGE_PRESENT_MASK, "entry 13: page is present");
	test_equals_int64(pageTable1.entries[13] & PAGE_READWRITE_MASK, PAGE_READWRITE_MASK, "entry 13: page is writable");
	test_equals_int64(pageTable1.entries[13] & PAGE_USERMODE_MASK, PAGE_USERMODE_MASK, "entry 13: page is user mode");
	test_equals_int64(pageTable1.entries[13] & PAGE_ACCESSED_MASK, 0, "entry 13: page has not been accessed");
	return test_end();
}

