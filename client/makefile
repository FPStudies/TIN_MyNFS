.DEFAULT_GOAL := all

CFLAGS := -Wextra -pedantic -std=c++17 -Wno-unused-variable

clientprotocol.o: clientprotocol.hpp clientprotocol.cpp
	g++ $(CFLAGS) -c clientprotocol.cpp -o clientprotocol.o

datagrams.o: datagrams.hpp datagrams.cpp
	g++ $(CFLAGS) -c datagrams.cpp -o datagrams.o

clientapi.o: clientapi.hpp clientapi.cpp clientprotocol.o datagrams.o
	g++ $(CFLAGS) -c clientapi.cpp -o clientapi.o

main.o: main.cpp clientapi.o
	g++ $(CFLAGS) -c main.cpp -o main.o

all: main.o
	g++ $(CFLAGS) -o client main.o clientapi.o clientprotocol.o datagrams.o

clean:
	rm -f *.o