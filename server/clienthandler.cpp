#include <iostream>
#include "clienthandler.hpp"

ClientHandler::ClientHandler(int clientNum, int sock) : clientNum(clientNum), sock(sock)
{

}

void ClientHandler::run()
{
    bool run = true;
    int readFlag;
    int fd;
    int id;
    ApiIDS apiID;
    std::cout << "Polaczono klienta nr "<< clientNum << ". Socket " << sock << std::endl;
    while(run)
    {
        char buffer[16];
        readFlag = readProtocol(buffer, sizeof(buffer));
        if (readFlag <= 0)
            run = false;
        else
        {
            id = datagrams.deserializeInt(&buffer[0], 1);
            apiID = static_cast<ApiIDS>(id);
            switch (apiID)
            {
                case ApiIDS::OPEN:
                    openFile(buffer);
                    break;
                case ApiIDS::READ:
                    readFile(buffer);
                    break;
                case ApiIDS::WRITE:
                    writeFile(buffer);
                    break;
                case ApiIDS::LSEEK:
                    lseekFile(buffer);
                    break;
                case ApiIDS::CLOSE:
                    closeFile(buffer);
                    break;
                case ApiIDS::CLOSEDIR:
                    closeDir(buffer);
                    break;
                case ApiIDS::READDIR:
                    readDir(buffer);
                    break;                
            }
        }
    }
}

void ClientHandler::openFile(char * buffer)
{
    int oflag = datagrams.deserializeInt(&buffer[4], 4);
    int mode = datagrams.deserializeInt(&buffer[8], 4);
    int pathLength = datagrams.deserializeInt(&buffer[12], 4);

    char * path = new char[pathLength + 1]; // Tu bedziecie miec sciezke do pliku
    readProtocol(path, pathLength + 1);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    int retVal = -1;
    int errorID = 2;

    char retPacket[8];
    datagrams.serializeInt(&retPacket[0], (int)ApiIDS::OPEN, 1); 
    datagrams.serializeInt(&retPacket[1], errorID, 1);
    retPacket[2] = 0;  
    retPacket[3] = 0; 
    datagrams.serializeInt(&retPacket[4], retVal, 4);

    sendProtocol(&retPacket[0], sizeof(retPacket));
    delete path;
}

void ClientHandler::readFile(char * buffer)
{
    int fd = datagrams.deserializeInt(&buffer[2], 2);
    int length = datagrams.deserializeInt(&buffer[4], 4);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    int errorID = -1;
    char * readString = "DUPADUPA22"; // to ma byc zwrocony string
    length = 11; // testowa wartosc. usunac

    char * retPacket = new char[8 + length];
    datagrams.serializeInt(&retPacket[0], (int)ApiIDS::READ, 1); 
    datagrams.serializeInt(&retPacket[1], errorID, 1);
    retPacket[2] = 0;
    retPacket[3] = 0;
    datagrams.serializeInt(&retPacket[4], length, 4);
    strcpy(&retPacket[8], readString);
    sendProtocol(&retPacket[0], 8 + length);
}

void ClientHandler::writeFile(char * buffer)
{
    int fd = datagrams.deserializeInt(&buffer[2], 2);
    int writeSize = datagrams.deserializeInt(&buffer[4], 4);

    char * toWrite = new char[writeSize + 1]; // Tu bedziecie miec sciezke do pliku
    readProtocol(toWrite, writeSize + 1);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    int retVal = -1;
    int errorID = 2;

    char retPacket[8];
    datagrams.serializeInt(&retPacket[0], (int)ApiIDS::WRITE, 1); 
    datagrams.serializeInt(&retPacket[1], errorID, 1);
    retPacket[2] = 0;  
    retPacket[3] = 0; 
    datagrams.serializeInt(&retPacket[4], retVal, 4);

    sendProtocol(&retPacket[0], sizeof(retPacket));
    delete toWrite;
}

void ClientHandler::lseekFile(char * buffer)
{
    int fd = datagrams.deserializeInt(&buffer[2], 2);
    int whence = datagrams.deserializeInt(&buffer[4], 4);
    int offset = datagrams.deserializeInt(&buffer[8], 4);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    int retVal = -1;
    int errorID = 5;

    char retPacket[8];
    datagrams.serializeInt(&retPacket[0], (int)ApiIDS::LSEEK, 1); 
    datagrams.serializeInt(&retPacket[1], errorID, 1);
    retPacket[2] = 0;   // Padding zgodnie z dokumentacja
    retPacket[3] = 0; 
    datagrams.serializeInt(&retPacket[4], retVal, 4);

    sendProtocol(&retPacket[0], sizeof(retPacket));
}

void ClientHandler::closeFile(char * buffer)
{
    int fd = datagrams.deserializeInt(&buffer[2], 2);

    // wywolac funkcje z fd tym
    // ustawic parametry nizej:

    int retVal = -1;
    int errorID = 10;

    char retPacket[8];
    datagrams.serializeInt(&retPacket[0], (int)ApiIDS::CLOSE, 1); 
    datagrams.serializeInt(&retPacket[1], errorID, 1);
    retPacket[2] = 0;   // Padding zgodnie z dokumentacja
    retPacket[3] = 0; 
    datagrams.serializeInt(&retPacket[4], retVal, 4);

    sendProtocol(&retPacket[0], sizeof(retPacket));
}

void ClientHandler::closeDir(char * buffer)
{
    int fd = datagrams.deserializeInt(&buffer[2], 2);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    int retVal = -1;
    int errorID = 2;

    char retPacket[8];
    datagrams.serializeInt(&retPacket[0], (int)ApiIDS::CLOSEDIR, 1); 
    datagrams.serializeInt(&retPacket[1], errorID, 1);
    retPacket[2] = 0;   // Padding zgodnie z dokumentacja
    retPacket[3] = 0; 
    datagrams.serializeInt(&retPacket[4], retVal, 4);

    sendProtocol(&retPacket[0], sizeof(retPacket));
}

void ClientHandler::readDir(char * buffer)
{
    int fd = datagrams.deserializeInt(&buffer[2], 2);

    // wywolac funkcje z fd tym
    // ustawic parametry nizej:

    int errorID = -1; // Uznałem, że -1 to nie ma błędu, bo od zera indeksujemy bledy w enum
    int stringLength = 11;
    char * returnString = "DUPADUPA22";

    char * retPacket;
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
    strcpy(&retPacket[8], returnString);
    sendProtocol(&retPacket[0], 8 + stringLength);

    delete retPacket;
}

int ClientHandler::readProtocol(char * buffer, std::size_t size)
{
    int readFlag;
    
    if ((readFlag = read(sock, buffer, size)) == -1)
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

int ClientHandler::sendProtocol(char * buffer, std::size_t size)
{
    int writeFlag;
    writeFlag = send(sock, buffer, size, 0);

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

