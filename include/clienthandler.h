/**
 * @file clienthandler.h
 * @author Mateusz Kordowski, Maciej Adamski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once


#include <sys/socket.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstddef>
#include <cstring>

#include "datagrams.h"

enum class ApiIDS: char
{
    OPEN = 2,
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

    void openFile(Deserialize& data);
    void readFile(Deserialize& data);
    void writeFile(Deserialize& data);
    void lseekFile(Deserialize& data);
    void closeFile(Deserialize& data);
    void closeDir(Deserialize& data);
    void readDir(Deserialize& data);
};
