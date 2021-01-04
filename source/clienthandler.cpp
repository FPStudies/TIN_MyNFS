/**
 * @file clienthandler.cpp
 * @author Mateusz Kordowski, Maciej Adamski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <iostream>
#include "clienthandler.h"

ClientHandler::ClientHandler(int clientNum, int sock) : clientNum(clientNum), sock(sock)
{

}

void ClientHandler::run()
{
    bool run = true;
    int readFlag;
    int fd;
    ApiIDS id;
    std::cout << "Polaczono klienta nr "<< clientNum << ". Socket " << sock << std::endl;
    while(run)
    {
        Deserialize data(16);
        readFlag = data.receiveData(sock, clientNum);
        if (readFlag <= 0)
            run = false;
        else
        {
            id = static_cast<ApiIDS>(data.deserializeChar());
            std::cout << (int)id << std::endl;
            switch (id)
            {
                case ApiIDS::OPEN:
                    openFile(data);
                    break;
                case ApiIDS::READ:
                    readFile(data);
                    break;
                case ApiIDS::WRITE:
                    writeFile(data);
                    break;
                case ApiIDS::LSEEK:
                    lseekFile(data);
                    break;
                case ApiIDS::CLOSE:
                    closeFile(data);
                    break;
                case ApiIDS::CLOSEDIR:
                    closeDir(data);
                    break;
                case ApiIDS::READDIR:
                    readDir(data);
                    break;       
                default:
                    std::cout << "nieznana operacja\n"; // TODO         
            }
        }
    }
}

void ClientHandler::openFile(Deserialize& data)
{
    OpenFileRecData received;
    data.castBufferToStruct(received);

    Deserialize retString(received.pathLength);
    retString.receiveData(sock, clientNum);
    char* path = new char[received.pathLength];
    retString.deserializeString(path, received.pathLength);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    DefRetIntSendData ret;
    ret.operID = static_cast<char>(ApiIDS::OPEN);
    ret.errorID = 2;
    ret.retVal = -1;
    Serialize::sendStruct(ret, sock, clientNum);

    delete path;
}

void ClientHandler::readFile(Deserialize& data)
{
    DefRecIntData rec;
    data.castBufferToStruct(rec);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    DefRetIntSendData ret;
    ret.operID = static_cast<char>(ApiIDS::READ);
    ret.errorID = 0;
    ret.retVal = 11;
    Serialize::sendStruct(ret, sock, clientNum);

    char * readString = "DUPADUPA22"; // to ma byc zwrocony string

    Serialize retString(ret.retVal);

    retString.serializeString(readString, ret.retVal);
    retString.sendData(sock, clientNum);
}

void ClientHandler::writeFile(Deserialize& data)
{
    DefRecIntData rec;
    data.castBufferToStruct(rec);
    // TODO wysłanie potwierdzenia, że przyjmiemy tyle bajtów
    Deserialize retString(rec.length);
    retString.receiveData(sock, clientNum);
    char* toWrite = new char[rec.length];// Tu bedziecie miec sciezke do pliku
    
    retString.deserializeString(toWrite, rec.length);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    DefRetIntSendData ret;
    ret.operID = static_cast<char>(ApiIDS::WRITE);
    ret.errorID = 2;
    ret.retVal = -1;
    Serialize::sendStruct(ret, sock, clientNum);

    delete toWrite;
}

void ClientHandler::lseekFile(Deserialize& data)
{
    LseekRecData rec;
    data.castBufferToStruct(rec);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    DefRetIntSendData ret;
    ret.operID = static_cast<char>(ApiIDS::LSEEK);
    ret.errorID = 0;
    ret.retVal = 5;
    Serialize::sendStruct(ret, sock, clientNum);
}

void ClientHandler::closeFile(Deserialize& data)
{
    RecDataOneLine rec;
    data.castBufferToStruct(rec);

    // wywolac funkcje z fd tym
    // ustawic parametry nizej:
    DefRetIntSendData ret;
    ret.operID = static_cast<char>(ApiIDS::CLOSE);
    ret.errorID = 0;
    ret.retVal = 10;
    Serialize::sendStruct(ret, sock, clientNum);
}

void ClientHandler::closeDir(Deserialize& data)
{
    RecDataOneLine rec;
    data.castBufferToStruct(rec);

    // Wywolac funkcje open z podanymi wyzej parametrami
    // ustawic te wartosci nizej:

    DefRetIntSendData ret;
    ret.operID = static_cast<char>(ApiIDS::CLOSEDIR);
    ret.errorID = 0;
    ret.retVal = 10;
    Serialize::sendStruct(ret, sock, clientNum);
}

void ClientHandler::readDir(Deserialize& data)
{
    RecDataOneLine rec;
    data.castBufferToStruct(rec);

    // wywolac funkcje z fd tym
    // ustawic parametry nizej:

    DefRetIntSendData ret;
    ret.operID = static_cast<char>(ApiIDS::READDIR);
    ret.errorID = 0;
    ret.retVal = 11;
    Serialize::sendStruct(ret, sock, clientNum);

    char * returnString = "DUPADUPA22";

    Serialize retString(ret.retVal);
    // TODO odebranie potwierdzenia od klienta odnośnie przesłania danych

    retString.serializeString(returnString, ret.retVal);
    retString.sendData(sock, clientNum);
}

