CC     = gcc
CFLAGS = -Wall -Wextra -g

all: test_preemptif

# preempt.c est inclus via #include dans test_preemptif.c
test_preemptif: mthread.c test_preemptif.c
	$(CC) $(CFLAGS) -o test_preemptif mthread.c test_preemptif.c

clean:
	rm -f test_preemptif

.PHONY: all clean
