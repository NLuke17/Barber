CC = gcc
CFLAGS = -Wall -pthread
TARGET = sleepingStylistSem

all: $(TARGET)

$(TARGET): sleepingStylistSem.c
	$(CC) $(CFLAGS) -o $(TARGET) sleepingStylistSem.c

clean:
	rm -f $(TARGET)