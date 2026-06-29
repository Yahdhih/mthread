CC     = gcc
CFLAGS = -Wall -Wextra -g

all: test_mutex

test_mutex: mthread.c mthread_mutex.c test_mutex.c
	$(CC) $(CFLAGS) -o test_mutex mthread.c mthread_mutex.c test_mutex.c

clean:
	rm -f test_mutex

.PHONY: all clean
