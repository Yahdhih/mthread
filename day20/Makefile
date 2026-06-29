CC     = gcc
CFLAGS = -Wall -Wextra -g

all: test_prod_conso

test_prod_conso: mthread.c mthread_sem.c test_prod_conso.c
	$(CC) $(CFLAGS) -o test_prod_conso mthread.c mthread_sem.c test_prod_conso.c

clean:
	rm -f test_prod_conso

.PHONY: all clean
