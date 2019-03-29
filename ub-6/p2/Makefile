CC=     gcc
testlib=../testlib
CFLAGS= -g -W -Wall -Werror -std=c99 -I$(testlib) -I.
LDFLAGS=-lrt
TARGET= message_queue
SRC=    message_queue.c testlib.c
VPATH=  $(testlib) tests
TESTS=  $(basename $(notdir $(wildcard tests/*.c)))

OBJ=      $(SRC:.c=.o)
OBJ-sol=  $(subst $(TARGET),$(TARGET)-solution,$(OBJ))
TESTS-sol=$(addsuffix -sol,$(TESTS))
testnames=$(1) $(addsuffix -sol,$(1))

all: $(TARGET) $(TESTS)

solution: $(TARGET)-sol $(TESTS-sol)

run-tests: $(TESTS)
	@bash $(testlib)/testrunner $(addprefix ./,$^)

run-tests-solution: $(TESTS-sol)
	@bash $(testlib)/testrunner $(addprefix ./,$^)

$(call testnames,test_exit): LDFLAGS+=-Wl,--wrap=mq_close -Wl,--wrap=mq_unlink

$(TARGET): main.o $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TESTS): %: %.o $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TARGET)-sol: main.o $(OBJ-sol)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TESTS-sol): %-sol: %.o $(OBJ-sol)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(TESTS) $(TESTS-sol) $(TARGET)-sol *.o

.PHONY: all solution run-tests run-tests-solution clean
