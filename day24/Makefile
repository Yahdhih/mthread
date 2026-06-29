CC     = gcc
CFLAGS = -Wall -Wextra -g

SRCS = mthread.c mthread_mutex.c mthread_cond.c

all: test_prod_conso_cond

test_prod_conso_cond: $(SRCS) test_prod_conso_cond.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f test_prod_conso_cond *.o

.PHONY: all clean
