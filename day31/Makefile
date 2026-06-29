CC     = gcc
CFLAGS = -Wall -Wextra -g

all: test_threads

test_threads: mthread.c test_threads.c
	$(CC) $(CFLAGS) -o test_threads mthread.c test_threads.c

valgrind: test_threads
	valgrind --leak-check=full ./test_threads

clean:
	rm -f test_threads

.PHONY: all valgrind clean
