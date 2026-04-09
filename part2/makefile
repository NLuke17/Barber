CC = gcc
CFLAGS = -Wall -pthread
TARGET = sleepingStylistMon

all: $(TARGET)

# Link the object file and main program [cite: 120]
$(TARGET): sleepingStylistMon.o monitor.o
	$(CC) $(CFLAGS) -o $(TARGET) sleepingStylistMon.o monitor.o

# Compile the monitor to an object file using the -c flag [cite: 119]
monitor.o: monitor.c
	$(CC) $(CFLAGS) -c monitor.c

sleepingStylistMon.o: sleepingStylistMon.c
	$(CC) $(CFLAGS) -c sleepingStylistMon.c

clean:
	rm -f $(TARGET) *.o