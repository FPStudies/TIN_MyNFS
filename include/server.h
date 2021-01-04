/**
 * @file server.h
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

void *clientThread(void *arg);

class Server
{
public:
    Server() {};
    void run();
private:
    static const int MAX_USER = 40;
    static const int PORT = 21370;
    int connectSocket = -1;
    struct sockaddr_in serverAddr;

    void createConnectSocket();
    void bindPort();
    void bindSocket();
    void setListen();
    void connectLoop();
    int createUserSocket();
};