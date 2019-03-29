#include "testlib.h"
#include "tslock.h"

int main() {
	test_start("You increment the ticket counter when getting a lock.");

	TicketSpinlock m;

	tslock_init(&m);
	int initialTicket = m.currentTicket;

	tslock_lock(&m);

	test_equals_int(m.currentTicket, initialTicket, "currentTicket is unchanged");
	test_equals_int(m.ticketCounter, initialTicket+1, "ticketCounter is incremented");

	return test_end();
}

