CC = gcc
CFLAGS = -Wall -pthread

all: sleepingStylistMon

sleepingStylistMon: sleepingStylistMon.c monitor.o
	$(CC) $(CFLAGS) -o sleepingStylistMon sleepingStylistMon.c monitor.o

monitor.o: monitor.c monitor.h
	$(CC) $(CFLAGS) -c monitor.c

clean:
	rm -f *.o sleepingStylistMon