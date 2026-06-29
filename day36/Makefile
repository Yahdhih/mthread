CC=gcc
CFLAGS=-Wall -Wextra -g
all: test_philosophes
test_philosophes: mthread.c mthread_mutex.c test_philosophes.c
	$(CC) $(CFLAGS) -o test_philosophes mthread.c mthread_mutex.c test_philosophes.c
clean:
	rm -f test_philosophes
.PHONY: all clean
