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
#include "API.h"
#include "LogUtils.h"

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

    void openFile(Deserialize& data, FDManager& manager, IDGen& gen);
    void openDir(Deserialize& data, FDManager& manager, IDGen& gen);
    void readFile(Deserialize& data, FDManager& manager);
    void writeFile(Deserialize& data, FDManager& manager);
    void lseekFile(Deserialize& data, FDManager& manager);
    void closeFile(Deserialize& data, FDManager& manager, bool& run);
    void unlink(Deserialize& data, FDManager& manager);
    void closeDir(Deserialize& data, FDManager& manager, bool& run);
    void readDir(Deserialize& data, FDManager& manager);
    void fstatFile(Deserialize& data, FDManager& manager);
};
