#include "testlib.h"
#include "tslock.h"

int main() {
	test_start("You increment the current ticket in your unlock.");

	TicketSpinlock m;

	tslock_init(&m);
	int initialTicket = m.currentTicket;

	tslock_lock(&m);
	tslock_unlock(&m);

	test_equals_int(m.currentTicket, initialTicket+1, "currentTicket is incremented");
	test_equals_int(m.ticketCounter, initialTicket+1, "ticketCounter is incremented");


	return test_end();
}

