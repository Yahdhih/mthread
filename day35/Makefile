CC=gcc
CFLAGS=-Wall -Wextra -g
all: test_stress
test_stress: mthread.c test_stress.c
	$(CC) $(CFLAGS) -o test_stress mthread.c test_stress.c
clean:
	rm -f test_stress
.PHONY: all clean
