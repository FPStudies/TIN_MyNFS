#pragma once
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstddef>
#include <cstring>
#include <memory>

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
        int bufSize = 4;
        void* buffor = new char[bufSize];
        int fd;
        char id;
        size_t bufPos = 0;
        std::cout << "Polaczono chuja nr "<< clientNum << ". Socket " << sock << std::endl;
         while(run)
         {
             bufPos = 0;
             
            readFlag = readProtocol(buffor, bufSize);

            if (readFlag <= 0)
                run = false;
            else
            {
                std::cout << "przeczytano " << readFlag << std::endl;
                id = datagrams.deserializeChar(buffor, bufPos);
                switch (id)
                {
                    case static_cast<char>(ApiIDS::CLOSEDIR):
                        closedir(buffor, bufPos);
                        break;
                    case static_cast<char>(ApiIDS::READDIR):
                        readdir(buffor, bufPos);
                        break;
                    default:
                        id = datagrams.deserializeChar(buffor, bufPos);
                        std::cout << "nieznana operacja " << (int)id << std::endl;
                }
            }
            sleep(1);
         }
        delete[] buffor;
    }

    void closedir(void* buffor, size_t bufPos)
    {
        std::cout << "CloseDir\n";
        datagrams.deserializePadding(buffor, 1, bufPos);
        short int fd = datagrams.deserializeShortInt(buffor, bufPos);
        int retVal = 0; // Dane testowe - trzeba zmienic
        char errorID = 0;

        // Tu wywołać zrobione funkcje 
        // retVal to zwrócona przez funkcje
        // a error sciagnac z tego errno jakoś

        size_t retPacketSize = 8;
        size_t retPos = 0;
        size_t padding = 2;
        void* retPacket = new char[retPacketSize];
        datagrams.serializeChar(retPacket, static_cast<char>(ApiIDS::CLOSEDIR), retPos); 
        datagrams.serializeChar(retPacket, errorID, retPos);
        datagrams.serializePadding(retPacket, padding, retPos); // Padding zgodnie z dokumentacja
        datagrams.serializeInt(retPacket, retVal, retPos);
        if(retPos != retPacketSize) {
            std::cout << "Błąd wielkości wysyłanego pakietu\n";
        }

        sendProtocol(retPacket, retPacketSize);
        delete[] retPacket;
    }

    void readdir(void* buffor, size_t bufPos)
    {
        std::cout << "ReadDir\n";
        datagrams.deserializePadding(buffor, 1, bufPos);
        short int fd = datagrams.deserializeShortInt(buffor, bufPos);
        char errorID = 0;
        int stringLength = 4;
        char * retPacket;
        size_t retPos = 0;
        char * testString = "abc";
        size_t retPacketSize = 0;

        std::cout << (int)fd << std::endl;

        if (errorID < 0) // jest błąd
        {
            retPacketSize = 8;
            retPacket = new char[retPacketSize];

        }
        else // brak błędu
        {
            retPacketSize = 8 + stringLength;
            retPacket = new char[retPacketSize];
        }
        
        datagrams.serializeChar(retPacket, static_cast<char>(ApiIDS::READDIR), retPos); 
        datagrams.serializeChar(retPacket, errorID, retPos);
        datagrams.serializePadding(retPacket, 2, retPos); // Padding zgodnie z dokumentacja
        

        if (errorID < 0) // jest błąd
        {
            datagrams.serializeInt(retPacket, 0, retPos); // długość stringa to 0
        }
        else // brak błędu
        {
            datagrams.serializeInt(retPacket, stringLength, retPos);
            datagrams.serializeString(retPacket, testString, stringLength, retPos);
        }
        
        sendProtocol(retPacket, retPacketSize);

        delete[] retPacket;
    }


    int readProtocol(void* buffor, std::size_t size)
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

    void clearSocketBuf(){
        char buf;
        int tmp;
        do{
            tmp = read(sock, &buf, 1);
        }
        while(tmp != 0 || tmp != -1);
    }

    int sendProtocol(void* buffor, std::size_t size)
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