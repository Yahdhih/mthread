CC      = gcc
CFLAGS  = -Wall -Wextra -g
LDFLAGS = -lpthread

all: observer observer_race

observer: observer.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

observer_race: observer_race.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

helgrind: observer_race
	valgrind --tool=helgrind ./observer_race

leak: observer
	valgrind --leak-check=full ./observer

clean:
	rm -f observer observer_race
