CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude
AR      = ar
ARFLAGS = rcs

SRCS = src/mthread.c src/mthread_mutex.c src/mthread_sem.c \
       src/mthread_cond.c src/mthread_preempt.c
OBJS = $(SRCS:.c=.o)
LIB  = libmthread.a

TESTS = tests/test_threads tests/test_mutex tests/test_sem tests/test_cond

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^
	@echo "==> $(LIB) créée"

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

tests/%: tests/%.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $< -L. -lmthread

test: $(TESTS)
	@echo "=== Lancement des tests ==="
	@for t in $(TESTS); do \
	    printf "%-30s" "$$t ..."; \
	    ./$$t > /dev/null 2>&1 && echo "[PASS]" || echo "[FAIL]"; \
	done

valgrind: $(TESTS)
	@for t in $(TESTS); do \
	    echo "=== valgrind $$t ==="; \
	    valgrind --leak-check=full --error-exitcode=1 ./$$t; \
	done

clean:
	rm -f $(OBJS) $(LIB) $(TESTS)

.PHONY: all test valgrind clean
