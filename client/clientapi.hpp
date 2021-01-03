#pragma once
#include <iostream>
#include <map>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <limits>
#include "clientprotocol.hpp"
#include "datagrams.hpp"


enum class ApiIDS
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
    //Client client;
    Datagrams datagrams;
    std::map<int, Client*> clients;
    
    ClientApi()
    {
        
    };

    int mynfs_open(char * host, char * path, int oflag, int mode)
    {
        Client * client = nullptr;

        // Szukamy czy mamy już takie polaczenie
        for (auto const& [key, val] : clients)
        {
            if (strcmp(val->getAddress(), host) == 0)
            {
                client = clients[key];
                break;
            }
        } 

        if (client == nullptr)
        {
            // otwiweramy nowe
            client = new Client(host);
            client->startConnection();
        }

        int fd = 5; // pobrac z serwera
        clients.insert(std::pair<int, Client*> (fd, client));

        return fd;
    }

    int mynfs_closedir(int dirfd)
    {
        if(dirfd > INT16_MAX || dirfd < INT16_MIN){ // tylko 16 bitów
            return -1;
        }

        if (clients.find(dirfd) == clients.end())
        {
            // nie istnieje taki
            // ustawic jakies errno o zlym deskryptorze
            return -1;
        }
        short int fd = static_cast<short int>(dirfd);

        Client * client = clients[fd];
        size_t sendSize = 4;
        size_t sendPos = 0;
        void* clientSendMSG = new char[sendSize];


        datagrams.serializeChar(clientSendMSG, static_cast<char>(ApiIDS::CLOSEDIR), sendPos);
        datagrams.serializePadding(clientSendMSG, 1, sendPos);
        datagrams.serializeShortInt(clientSendMSG, fd, sendPos);

        client->sendProtocol(clientSendMSG, sendSize);

        /////////////////////////////////////////////

        size_t retSize = 8;
        size_t retPos = 0;
        void* returnBuffer = new char[retSize];
        int readFlag = client->readProtocol(returnBuffer, retSize);

        // Returned ma zwrócone wartości przez serwer
        char operID = datagrams.deserializeChar(returnBuffer, retPos);
        char errorID = datagrams.deserializeChar(returnBuffer, retPos);
        datagrams.deserializePadding(returnBuffer, 2, retPos);
        int reply = datagrams.deserializeInt(returnBuffer, retPos);
        
        if(operID != static_cast<char>(ApiIDS::CLOSEDIR)){
            delete[] clientSendMSG;
            delete[] returnBuffer;
            return -1; // nie ta operacja
        }

        if(errorID != 0){
            delete[] clientSendMSG;
            delete[] returnBuffer;
            return -1; // błąd
        }

        std::cout << "Zwrocono ret: " << reply << ", error: " << errorID << std::endl;
        delete[] clientSendMSG;
        delete[] returnBuffer;
        return reply;      

    };

    char * mynfs_readdir(int dirfd)
    {
        if(dirfd > INT16_MAX || dirfd < INT16_MIN){ // tylko 16 bitów
            return NULL;
        }
        short int fd = static_cast<short int>(dirfd);

        if (clients.find(dirfd) == clients.end())
        {
            // nie istnieje taki
            // ustawic jakies errno o zlym deskryptorze
            return NULL;
        }

        Client * client = clients[dirfd];
        size_t sendSize = 8;
        size_t sendPos = 0;
        void* clientSendMSG = new char[sendSize];
        datagrams.serializeChar(clientSendMSG, static_cast<char>(ApiIDS::READDIR), sendPos);
        datagrams.serializePadding(clientSendMSG, 1, sendPos);
        datagrams.serializeShortInt(clientSendMSG, fd, sendPos);

        client->sendProtocol(clientSendMSG, sendSize);

        size_t retSize = 4096;
        size_t retPos = 0;
        size_t retHeaderSize = 8;
        void* returnBuffer = new char[retSize];
        int readFlag = client->readProtocol(returnBuffer, retHeaderSize);
        
        char operID = datagrams.deserializeChar(returnBuffer, retPos);
        char errorID = datagrams.deserializeChar(returnBuffer, retPos);
        datagrams.deserializePadding(returnBuffer, 2, retPos);
        int strSize = datagrams.deserializeInt(returnBuffer, retPos);

        if(operID != static_cast<char>(ApiIDS::READDIR)){
            delete[] clientSendMSG;
            delete[] returnBuffer;
            return NULL; // nie ta operacja
        }

        if(errorID != 0){
            std::cout << "Zwrocono error: " << errorID << std::endl;
            delete[] clientSendMSG;
            delete[] returnBuffer;
            return NULL; // błąd
        }

        if(strSize <= 0){
            delete[] clientSendMSG;
            delete[] returnBuffer;
            return NULL;
        }
        // else

        char* resStr = new char[strSize];
        retPos = 0;
        readFlag = client->readProtocol(returnBuffer, strSize);
        datagrams.deserializeString(returnBuffer, resStr, strSize, retPos);

        delete[] returnBuffer;
        return resStr;
    }

    int mynfs_opendir(char * host, char *path)
    {
        return mynfs_open(host, path, O_DIRECTORY, 0);
    }
};