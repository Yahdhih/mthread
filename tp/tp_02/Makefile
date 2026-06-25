CC     = gcc
CFLAGS = -Wall -Wextra -g

all: pingpong context_arg chain

pingpong: pingpong.c
	$(CC) $(CFLAGS) -o $@ $<

context_arg: context_arg.c
	$(CC) $(CFLAGS) -o $@ $<

chain: chain.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	@echo "=== Test pingpong ===" && ./pingpong
	@echo "=== Test context_arg ===" && ./context_arg

clean:
	rm -f pingpong context_arg chain
