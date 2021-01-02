.DEFAULT_GOAL := all

CFLAGS := -Wall -Wextra -pedantic -pthread -std=c++17

datagrams.o: datagrams.hpp datagrams.cpp
	g++ $(CFLAGS) -c datagrams.cpp -o datagrams.o

clienthandler.o: clienthandler.cpp clienthandler.hpp datagrams.o
	g++ $(CFLAGS) -c clienthandler.cpp -o clienthandler.o

server.o: server.hpp server.cpp clienthandler.o
	g++ $(CFLAGS) -c server.cpp -o server.o

main.o: main.cpp server.o
	g++ $(CFLAGS) -c main.cpp -o main.o

all: main.o
	g++ $(CFLAGS) -o server main.o server.o