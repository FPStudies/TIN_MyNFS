#pragma once
#include <iostream>
#include <map>
#include <cstring>
#include <fcntl.h>
#include <string>
#include "clientprotocol.hpp"
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
        if (clients.find(dirfd) == clients.end())
        {
            // nie istnieje taki
            // ustawic jakies errno o zlym deskryptorze
            return -1;
        }

        Client * client = clients[dirfd];
        char clientSendMSG[4];

        clientSendMSG[0] = (int)ApiIDS::CLOSEDIR;
        clientSendMSG[1] = 0; // padding
        datagrams.serializeInt(&clientSendMSG[2], dirfd, 2);

        client->sendProtocol(&clientSendMSG[0], sizeof(clientSendMSG));

        char returnBuffer[9];
        int readFlag = client->readProtocol(returnBuffer, sizeof(returnBuffer));

        // Returned ma zwrócone wartości przez serwer
        int retVal = datagrams.deserializeInt(&returnBuffer[4], 4);
        int errorID = 0;
        if (retVal == -1)
        {
            errorID = datagrams.deserializeInt(&returnBuffer[1], 1);

            // Tu ustawić errno na errorID
        }
        std::cout << "Zwrocono ret: " << retVal << ", error: " << errorID << std::endl;

        return retVal;
    };

    char * mynfs_readdir(int dirfd)
    {
        if (clients.find(dirfd) == clients.end())
        {
            // nie istnieje taki
            // ustawic jakies errno o zlym deskryptorze
            return NULL;
        }

        Client * client = clients[dirfd];
        char clientSendMSG[4];
        clientSendMSG[0] = (int)ApiIDS::READDIR;
        clientSendMSG[1] = 0; // padding
        datagrams.serializeInt(&clientSendMSG[2], dirfd, 2);

        client->sendProtocol(&clientSendMSG[0], sizeof(clientSendMSG));

        char returnBuffer[4104];
        int readFlag = client->readProtocol(returnBuffer, sizeof(returnBuffer));
        
        int errorID = datagrams.deserializeInt(&returnBuffer[1], 1);

        if (errorID == -1)
        {
            // errorID = -1 to oznacza brak bledu i przyjmujemy string
            int stringLength = datagrams.deserializeInt(&returnBuffer[4], 4);
            char * test = new char[stringLength];
            char * dupa = datagrams.deserializeChar(returnBuffer, stringLength);
            strcpy(test, returnBuffer+8);

            // Powinien być ciag znakow od 8 do 16 chara
        }
        else
        {
            // inaczej ustawiamy errno odpowiednie z errorID
            return NULL;
        }
        
        std::cout << "Zwrocono error: " << errorID << std::endl;

        return NULL;
    }

    int mynfs_opendir(char * host, char *path)
    {
        return mynfs_open(host, path, O_DIRECTORY, 0);
    }
};