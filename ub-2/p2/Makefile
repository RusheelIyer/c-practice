CC=     gcc
testlib=../testlib
CFLAGS= -g -W -Wall -Werror -std=c99 -I$(testlib) -I.
LDFLAGS=
TARGET= run_program
SRC=    run_program.c testlib.c
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

wrap := -Wl,--wrap=
$(call testnames,test_exec): LDFLAGS+=$(addprefix $(wrap),fork execv execvp wait waitpid)
$(call testnames,test_wait test_exitstatus test_error): LDFLAGS+=$(addprefix $(wrap),fork wait waitpid)

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
