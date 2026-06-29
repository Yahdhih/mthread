CC     = gcc
CFLAGS = -Wall -Wextra -g

all: bench_mthread bench_pthread

bench_mthread: mthread.c mthread_mutex.c benchmark.c
	$(CC) $(CFLAGS) -DUSE_MTHREAD -o bench_mthread mthread.c mthread_mutex.c benchmark.c

bench_pthread: benchmark.c
	$(CC) $(CFLAGS) -DUSE_PTHREAD -o bench_pthread benchmark.c -lpthread

clean:
	rm -f bench_mthread bench_pthread

.PHONY: all clean
