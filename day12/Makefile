CC = gcc
CFLAGS = -Wall -Wextra -g

all: test_yield test_join

test_yield: mthread.c test_yield.c
	$(CC) $(CFLAGS) -o test_yield mthread.c test_yield.c

test_join: mthread.c test_join.c
	$(CC) $(CFLAGS) -o test_join mthread.c test_join.c

clean:
	rm -f test_yield test_join

.PHONY: all clean
