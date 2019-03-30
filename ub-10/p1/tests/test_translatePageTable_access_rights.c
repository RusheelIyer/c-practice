#include "testlib.h"
#include "page_table.h"
#include <stdio.h>

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
PageTable __attribute__((aligned(0x1000))) pageTable1;

int main() {
	test_start("Your translatePageTable respects access rights.");

	basePageDirectory.entries[1] = pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;

	pageTable1.entries[1] =0x2000 | PAGE_PRESENT_MASK;
	pageTable1.entries[3] =0x3000 | PAGE_PRESENT_MASK | PAGE_READWRITE_MASK;
	pageTable1.entries[4] =0x4000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK;
	pageTable1.entries[5] =0x5000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK | PAGE_READWRITE_MASK;
	setPageDirectory(&basePageDirectory);

	uint32_t address = (1 * ENTRIES_PER_TABLE + 1) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_READ, KERNEL_MODE), 0, "1: translatePageTable succeeds with read access from kernel mode");
	setPageDirectory(&basePageDirectory); // <- to not crash TLB because it gets the same entry twice.
	address = (1 * ENTRIES_PER_TABLE + 1) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_WRITE, KERNEL_MODE), 0, "1: translatePageTable succeeds with write access from kernel mode");
	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 1) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_READ, USER_MODE), -1, "1: translatePageTable fails with read access from user mode");
	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 1) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_WRITE, USER_MODE), -1, "1: translatePageTable fails with write access from user mode");

	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 4) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_READ, KERNEL_MODE), 0, "4: translatePageTable succeeds with read access from kernel mode");
	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 4) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_WRITE, KERNEL_MODE), 0, "4: translatePageTable succeeds with write access from kernel mode");
	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 4) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_READ, USER_MODE), 0, "4: translatePageTable succeeds with read access from user mode");
	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 4) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_WRITE, USER_MODE), -1, "4: translatePageTable fails with write access from kernel mode");

	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 5) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_READ, KERNEL_MODE), 0, "5: translatePageTable succeeds with read access from kernel mode");
	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 5) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_WRITE, KERNEL_MODE), 0, "5: translatePageTable succeeds with write access from kernel mode");
	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 5) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_READ, USER_MODE), 0, "5: translatePageTable succeeds with read access from user mode");
	setPageDirectory(&basePageDirectory);
	address = (1 * ENTRIES_PER_TABLE + 5) * (1<<OFFSET_BITS);
	test_equals_int(translatePageTable(&address, ACCESS_WRITE, USER_MODE), 0, "5: translatePageTable succeeds with write access from user mode");
	return test_end();
}

