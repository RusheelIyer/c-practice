#include "testlib.h"
#include "tslock.h"

int main() {
	test_start("You initialize the lock correctly.");

	TicketSpinlock m = {3, 4};

	tslock_init(&m);

	// Although initializing to 0 is most natural here, any number will work.
	test_equals_int(m.currentTicket, m.ticketCounter, "currentTicket equals ticketCounter");

	return test_end();
}
