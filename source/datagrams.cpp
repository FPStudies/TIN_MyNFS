/**
 * @file datagrams.cpp
 * @author Mateusz Kordowski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "datagrams.h"

Datagram::Datagram(size_t bufSize) 
: pos(0), allDataSize(0), bufSize(bufSize), buffor(new char[bufSize]), del(true)
{}

Datagram::~Datagram() 
{
    if(del)
        delete (char*)buffor;
}

Deserialize::Deserialize(size_t bufSize)
: Datagram(bufSize)
{}

Serialize::Serialize(size_t bufSize)
: Datagram(bufSize)
{}

void Serialize::serializePadding(const size_t size){
    memset(buffor + pos, 0, size); // to not send trash data
    pos += size;
    allDataSize = std::max(pos, allDataSize);
}

void Serialize::serializeInt(const int value)
{
    memcpy(buffor + pos, &value, sizeof(int));
    pos += sizeof(int);
    allDataSize = std::max(pos, allDataSize);
}

void Serialize::serializeString(const char* string,const size_t size)
{
    if(size > bufSize)
        throw std::runtime_error("Buffer overflow.");
    memcpy(buffor + pos, string, size);
    pos += size;
    allDataSize = std::max(pos, allDataSize);
}

void Serialize::serializeChar(const char character)
{
    memcpy(buffor + pos, &character, sizeof(char));
    pos += sizeof(char);
    allDataSize = std::max(pos, allDataSize);
}  

void Serialize::serializeShortInt(const short int value)
{
    memcpy(buffor + pos, &value, sizeof(short int));
    pos += sizeof(short int);
    allDataSize = std::max(pos, allDataSize);
}

void Deserialize::deserializePadding(const size_t size){
    pos += size;
    allDataSize = std::max(pos, allDataSize);
}

int Deserialize::deserializeInt()
{
    int ret;
    memcpy(&ret, buffor + pos, sizeof(int));
    pos += sizeof(int);
    allDataSize = std::max(pos, allDataSize);
    return ret;
}

void Deserialize::deserializeString(char* string, const size_t strSize){
    char ret;
    if(strSize > bufSize)
        throw std::runtime_error("Buffer overflow.");
    memcpy(string, buffor + pos, strSize);
    pos += strSize;
    allDataSize = std::max(pos, allDataSize);
}

char Deserialize::deserializeChar(){
    char ret;
    memcpy(&ret, buffor + pos, sizeof(char));
    pos += sizeof(char);
    allDataSize = std::max(pos, allDataSize);
    return ret;
}

short int Deserialize::deserializeShortInt(){
    short int ret;
    memcpy(&ret, buffor + pos, sizeof(short int));
    pos += sizeof(short int);
    allDataSize = std::max(pos, allDataSize);
    return ret;
}

size_t Datagram::getPos() const{
    return pos;
}

void Datagram::setPos(size_t newPos){
    pos = newPos;
}

size_t Datagram::getDataSize() const{
    return allDataSize;
}

size_t Datagram::getBufSize() const{
    return bufSize;
}

ssize_t Deserialize::receiveData(const Client& client){
    ssize_t readFlag;

    if ((readFlag = read(client.getSocket(), buffor, bufSize)) == -1)
    {
        std::cout << "Nie udalo sie odebrac." << std::endl;
        return -1;
    }
    else if (readFlag == 0)
    {
        std::cout << "Koniec polaczenia z klientem."  << std::endl;
        return 0;
    }
    
    return readFlag;
}

ssize_t Serialize::sendData(const Client& client)
{
    ssize_t sendFlag;
    sendFlag = send(client.getSocket(), buffor, bufSize, 0);

    if (sendFlag == -1)
    {
        std::cout << "Nie udalo sie wyslac wiadomosci" << std::endl;
    }
    else
    {
        std::cout << "Wyslano wiadomosc" << std::endl;
    }
}