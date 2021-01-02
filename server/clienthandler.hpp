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
    ClientHandler(int clientNum, int sock) : clientNum(clientNum), sock(sock)
    {

    }
    int clientNum;
    int sock;
    Datagrams datagrams;
    

    void run()
    {
        bool run = true;
        int readFlag;
        char buffor[10];
        int fd;
        int id;
        std::cout << "Polaczono chuja nr "<< clientNum << ". Socket " << sock << std::endl;
        // while(run)
        // {
            readFlag = readProtocol(buffor, sizeof(buffor));

            if (readFlag <= 0)
                run = false;
            else
            {
                id = datagrams.deserializeInt(&buffor[0], 1);
                switch (id)
                {
                    case (int)ApiIDS::CLOSEDIR:
                        closedir(buffor);
                        break;
                    case (int)ApiIDS::READDIR:
                        readdir(buffor);
                        break;
                }
            }
        // }
    }

    void closedir(char * buffor)
    {
        int fd = datagrams.deserializeInt(&buffor[2], 2);
        int retVal = -1; // Dane testowe - trzeba zmienic
        int errorID = 2;

        // Tu wywołać zrobione funkcje 
        // retVal to zwrócona przez funkcje
        // a error sciagnac z tego errno jakoś

        char retPacket[9];
        datagrams.serializeInt(&retPacket[0], (int)ApiIDS::CLOSEDIR, 1); 
        datagrams.serializeInt(&retPacket[1], errorID, 1);
        retPacket[2] = 0;   // Padding zgodnie z dokumentacja
        retPacket[3] = 0; 
        datagrams.serializeInt(&retPacket[4], retVal, 4);

        sendProtocol(&retPacket[0], sizeof(retPacket));
    }

    void readdir(char * buffor)
    {
        int fd = datagrams.deserializeInt(&buffor[2], 2);
        int errorID = -1; // Uznałem, że -1 to nie ma błędu, bo od zera indeksujemy bledy w enum
        int stringLength = 10;
        char * retPacket;
        char * testString = "DUPADUPA22";

        if (errorID >= 0)
        {
            retPacket = new char[4];
        }
        else
        {
            retPacket = new char[8 + stringLength];
        }
        
        datagrams.serializeInt(&retPacket[0], (int)ApiIDS::READDIR, 1); 
        datagrams.serializeInt(&retPacket[1], errorID, 1);
        retPacket[2] = 0;   // Padding zgodnie z dokumentacja
        retPacket[3] = 0; 
        datagrams.serializeInt(&retPacket[4], stringLength, 4);

        strcpy(retPacket+8, testString);
        
        sendProtocol(&retPacket[0], sizeof(retPacket));
    }


    int readProtocol(char * buffor, std::size_t size)
    {
        int readFlag;
        
        if ((readFlag = read(sock, buffor, size)) == -1)
        {
            std::cout << "Nie udalo sie odebrac. (" << clientNum << ")" << std::endl;
            return -1;
        }
        else if (readFlag == 0)
        {
            std::cout << "Koniec polaczenia z klientem " << clientNum << std::endl;
            return 0;
        }
        
        return readFlag;
    }

    int sendProtocol(char * buffor, std::size_t size)
    {
        int writeFlag;
        writeFlag = send(sock, buffor, size, 0);
        
        if (writeFlag == -1)
        {
            std::cout << "Nie udalo sie wyslac wiadomosci" << std::endl;
        }
        else
        {
            std::cout << "Wyslano wiadomosc" << std::endl;
        }
        return writeFlag;
    }
};

void *clientHandlerThread(void *arg)
{
    static int clientNums = 1;
    int sock = *((int *) arg);
    std::cout << "Utworzono watek klienta nr " << clientNums << ", socket " << sock << std::endl;
    ClientHandler handler(clientNums++, sock);

    handler.run();

    std::cout << "Zamknieto socket " << sock << std::endl;
	close(sock);
	pthread_exit(NULL);
}