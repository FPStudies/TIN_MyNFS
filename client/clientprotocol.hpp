/**
 * @file clientprotocol.hpp
 * @author Maciej Adamski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

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
    void sendProtocol(char * buffor, std::size_t size);
    int readProtocol(char * buffor, std::size_t size);
    char * getAddress();
    int getSocket() const;
    
private:
    static const int PORT = 21370;
    int sock;
    struct sockaddr_in server;
    char * addr;
    void createSocket();
    void connectToServer();
    unsigned char * serializeInt(unsigned char * buffor, int value, int size);
};