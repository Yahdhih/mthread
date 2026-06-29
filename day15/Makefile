CC     = gcc
CFLAGS = -Wall -Wextra -g

all: test_counter

test_counter: mthread.c mthread_mutex.c test_counter.c
	$(CC) $(CFLAGS) -o test_counter mthread.c mthread_mutex.c test_counter.c

clean:
	rm -f test_counter

.PHONY: all clean
