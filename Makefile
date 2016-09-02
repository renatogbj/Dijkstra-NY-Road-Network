CC = g++
CFLAGS  = -g -Wall

default: dijkstra

dijkstra: dijkstra.o CPUTimer.o
	$(CC) $(CFLAGS) -o dijkstra dijkstra.o CPUTimer.o

CPUTimer.o: CPUTimer.cpp CPUTimer.h
	$(CC) $(CFLAGS) -c CPUTimer.cpp

dijkstra.o: dijkstra.cpp
	$(CC) $(CFLAGS) -c dijkstra.cpp

clean:
	$(RM) count *.o *~
