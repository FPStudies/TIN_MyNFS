#pragma once


#include <sys/socket.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstddef>
#include <cstring>

#include "datagrams.hpp"

enum class ApiIDS
{
    OPEN,
    READ,
    WRITE,
    LSEEK,
    CLOSE,
    UNLINK,
    FSTAT,
    OPENDIR,
    READDIR,
    CLOSEDIR
};

class ClientHandler
{
public:
    ClientHandler(int clientNum, int sock);
    void run();

private:
    int clientNum;
    int sock;
    Datagrams datagrams;

    void openFile(char * buffer);
    void readFile(char * buffer);
    void writeFile(char * buffer);
    void lseekFile(char * buffer);
    void closeFile(char * buffer);
    void closeDir(char * buffer);
    void readDir(char * buffer);

    int readProtocol(char * buffer, std::size_t size);
    int sendProtocol(char * buffbufferor, std::size_t size);
};
