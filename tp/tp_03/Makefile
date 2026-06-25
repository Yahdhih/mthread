CC      = gcc
CFLAGS  = -Wall -Wextra -g -fsanitize=address
SRCS    = mthread.c

all: test_yield test_join

test_yield: $(SRCS) test_yield.c
	$(CC) $(CFLAGS) -o $@ $^

test_join: $(SRCS) test_join.c
	$(CC) $(CFLAGS) -o $@ $^

test: all
	@echo "\n=== test_yield ===" && ./test_yield
	@echo "\n=== test_join  ===" && ./test_join

valgrind: all
	valgrind --leak-check=full ./test_yield
	valgrind --leak-check=full ./test_join

clean:
	rm -f test_yield test_join
