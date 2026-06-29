CC = gcc
CFLAGS = -Wall -Wextra -g

all: pingpong context_arg chain

pingpong: pingpong.c
	$(CC) $(CFLAGS) -o pingpong pingpong.c

context_arg: context_arg.c
	$(CC) $(CFLAGS) -o context_arg context_arg.c

chain: chain.c
	$(CC) $(CFLAGS) -o chain chain.c

clean:
	rm -f pingpong context_arg chain

.PHONY: all clean
