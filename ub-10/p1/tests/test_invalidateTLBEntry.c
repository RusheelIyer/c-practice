#include "testlib.h"
#include "page_table.h"

int main() {
	test_start("Your invalidateTLBEntry works.");

	addToTLB(0x1000, 0x5000 | PAGE_PRESENT_MASK);
	addToTLB(0x8000, 0x6000 | PAGE_PRESENT_MASK | PAGE_USERMODE_MASK);

	uint32_t address = 0x1000;
	test_equals_int(translateTLB(&address, ACCESS_READ, KERNEL_MODE), 0, "translateTLB succeeds");
	test_equals_int64(address, 0x5000, "address is correct");

	address = 0x8000 + 500;
	test_equals_int(translateTLB(&address, ACCESS_READ, KERNEL_MODE), 0, "translateTLB succeeds");
	test_equals_int64(address, 0x6000 + 500, "address is correct");

	invalidateTLBEntry(0x8000);

	address = 0x1000;
	test_equals_int(translateTLB(&address, ACCESS_READ, KERNEL_MODE), 0, "after invalidate: translateTLB succeeds");
	test_equals_int64(address, 0x5000, "address is correct");

	address = 0x8000 + 500;
	test_equals_int(translateTLB(&address, ACCESS_READ, KERNEL_MODE), -1, "after invalidate: translateTLB fails for invalidated page");
	test_equals_int64(address, 0x8000 + 500, "address is unchanged");

	return test_end();
}

