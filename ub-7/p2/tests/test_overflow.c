#include "testlib.h"
#include "tslock.h"

int main() {
	test_start("You can handle integer overflows.");

	TicketSpinlock m;

	tslock_init(&m);

	m.currentTicket = 2147483646;
	m.ticketCounter = 2147483646;

	tslock_lock(&m);
	tslock_unlock(&m);

	tslock_lock(&m);
	tslock_unlock(&m);

	tslock_lock(&m);
	tslock_unlock(&m);

	test_equals_int(m.currentTicket, -2147483647, "currentTicket is correct");
	test_equals_int(m.ticketCounter, -2147483647, "ticketCounter is correct");


	return test_end();
}

