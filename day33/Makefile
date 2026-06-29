CC=gcc
CFLAGS=-Wall -Wextra -g
all: test_sem
test_sem: mthread.c mthread_mutex.c mthread_sem.c test_sem.c
	$(CC) $(CFLAGS) -o test_sem mthread.c mthread_mutex.c mthread_sem.c test_sem.c
clean:
	rm -f test_sem
.PHONY: all clean
