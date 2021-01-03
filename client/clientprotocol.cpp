#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include "clientprotocol.hpp"


Client::Client(char * address)
{
    addr = address;
    server.sin_family = AF_INET;
    struct hostent* hp;
    hp = gethostbyname(address);
    if (hp == (struct hostent*) 0)
    {
        std::cout << "Nie odnaleziono hosta" << std::endl;
        exit(1);
    }

    memcpy((char*) &server.sin_addr, (char *) hp->h_addr, hp->h_length);

    server.sin_port = htons(PORT);
}

char * Client::getAddress()
{
    return addr;
}

void Client::startConnection()
{
    createSocket();
    connectToServer();
}

unsigned char * Client::serializeInt(unsigned char * buffor, int value, int size)
{
    for (int i = size; i > 0; i--)
    {
        buffor[size - i] = value >> (8 * (i -1));
    }

    return buffor + size;
}

void Client::createSocket()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        std::cout << "Blad podczas tworzenia socketu" << std::endl;
        exit(1);
    }
}

void Client::connectToServer()
{
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)))
    {
        std::cout << "Blad polaczenia z serwerem" << std::endl;
        exit(1);
    }

    std::cout << "Polaczono z serwerem." << std::endl;
}

ssize_t  Client::sendProtocol(void* buffor, std::size_t size)
{
    ssize_t ret = 0;
    if ((ret = send(sock, buffor, size, 0)) == -1)
    {
        std::cout << "Nie udalo sie wyslac wiadomosci" << std::endl;
    }
    else
    {
        std::cout << "Wyslano wiadomosc" << std::endl;
    }
    return ret;
}

int Client::readProtocol(void* buffor, std::size_t size)
{
        int readFlag;

        if ((readFlag = read(sock, buffor, size)) == -1)
        {
            std::cout << "Nie udalo sie odebrac." << std::endl;
            return -1;
        }
        else if (readFlag == 0)
        {
            std::cout << "Koniec polaczenia z klientem "  << std::endl;
            return 0;
        }

        return readFlag;
}