/**
 * @file clientapi.hpp
 * @author Mateusz Kordowski, Maciej Adamski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once
#include <iostream>
#include <map>
#include <cstring>
#include <fcntl.h>
#include <string>
#include "clientprotocol.hpp"
#include "datagrams.hpp"


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

class ClientApi
{
public:  
    ClientApi();

    int mynfs_open(char * host, char * path, int oflag, int mode);
    int mynfs_read(int mynfs_fd, char * buf, int len);
    int mynfs_write(int mynfs_fd, const char * buf, int len);
    int mynfs_lseek(int mynfs_fd, int whence, int offset);
    int mynfs_close(int mynfs_fd);
    int mynfs_closedir(int dirfd);
    char * mynfs_readdir(int dirfd);
    int mynfs_opendir(char *host, char *path);

private:
    std::map<int, Client*> clients;

    void setErrno(int errorID);
    bool clientExist(int fd);
};