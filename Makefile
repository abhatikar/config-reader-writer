# Usage:
# Run 'make test' to execute the test program.
# Run 'make valgrind' to run the test program in Valgrind.
# Run 'make clean' to remove compiled files.

CC			:= gcc
CFLAGS			:= -Wall -Wextra -std=c99 -g -D_XOPEN_SOURCE=500
SRC			:= prefs.c
DEPS			:= $(SRC) $(SRC:.c=.h)
TARGET			:= config-reader-writer

.PHONY: test valgrind submit clean

default: compile

compile:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test: compile
	./config-reader-writer

valgrind:
	valgrind --tool=memcheck ./test

clean:
	-rm -f *.o
	-rm -f $(TARGET) $(SRC:.c)
