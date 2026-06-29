CC     = gcc
CFLAGS = -Wall -Wextra -g

SRCS = mthread.c mthread_mutex.c mthread_sem.c mthread_cond.c

all: check_compile

check_compile: $(SRCS)
	$(CC) $(CFLAGS) -c mthread.c
	$(CC) $(CFLAGS) -c mthread_mutex.c
	$(CC) $(CFLAGS) -c mthread_sem.c
	$(CC) $(CFLAGS) -c mthread_cond.c
	@echo "Compilation OK"

clean:
	rm -f *.o

.PHONY: all check_compile clean
