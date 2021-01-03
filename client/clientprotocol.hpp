#pragma once 

#include <sys/types.h>
#include <netinet/in.h>
#include <cstddef>
#include <unistd.h> 
#include <iostream>
class Client
{
public:
    Client() {};
    Client(char * address);

    void startConnection();
    ssize_t sendProtocol(void* buffor, std::size_t size);
    int readProtocol(void* buffor, std::size_t size);
    char * getAddress();
private:
    static const int PORT = 21370;
    int sock;
    struct sockaddr_in server;
    char * addr;
    void createSocket();
    void connectToServer();
    unsigned char * serializeInt(unsigned char * buffor, int value, int size);
};