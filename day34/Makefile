CC=gcc
CFLAGS=-Wall -Wextra -g
all: test_cond
test_cond: mthread.c mthread_mutex.c mthread_cond.c test_cond.c
	$(CC) $(CFLAGS) -o test_cond mthread.c mthread_mutex.c mthread_cond.c test_cond.c
clean:
	rm -f test_cond
.PHONY: all clean
