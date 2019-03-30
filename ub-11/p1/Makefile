CC=     gcc
testlib=../testlib
CFLAGS= -g -W -Wall -Werror -std=c99 -I$(testlib) -I.
TARGET= page_faults
SRC=    page_faults.c testlib.c
VPATH=  $(testlib) tests
TESTS=  $(basename $(notdir $(wildcard tests/*.c)))

OBJ=      $(SRC:.c=.o)
OBJ-sol=  $(subst $(TARGET),$(TARGET)-solution,$(OBJ))
TESTS-sol=$(addsuffix -sol,$(TESTS))

all: $(TARGET) $(TESTS)

solution: $(TARGET)-sol $(TESTS-sol)

run-tests: $(TESTS)
	@bash $(testlib)/testrunner $(addprefix ./,$^)

run-tests-solution: $(TESTS-sol)
	@bash $(testlib)/testrunner $(addprefix ./,$^)

$(TARGET): main.o $(OBJ)
	$(CC) -o $@ $^

$(TESTS): %: %.o $(OBJ)
	$(CC) -o $@ $^

$(TARGET)-sol: main.o $(OBJ-sol)
	$(CC) -o $@ $^

$(TESTS-sol): %-sol: %.o $(OBJ-sol)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(TESTS) $(TESTS-sol) $(TARGET)-sol *.o

.PHONY: all solution run-tests run-tests-solution clean
