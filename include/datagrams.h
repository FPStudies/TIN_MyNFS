/**
 * @file datagrams.h
 * @author Mateusz Kordowski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once
#include <iostream>
#include <cstring>
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include "clientapi.h"

struct OpenFileRecData{
    char operID;
    short int fileDescriptor;
    int oflag;
    int mode;
    int pathLength;

    operator std::string();
};

struct ReadFileRecData{
    char operID;
    short int fileDescriptor;
    int length;

    operator std::string();
};

struct DefRecIntData{
    char operID;
    short int fileDescriptor;
    int length;

    operator std::string();
};


struct LseekRecData{
    char operID;
    short int fileDescriptor;
    int whence;
    int offset;

    operator std::string();
};

struct CloseRecData{
    char operID;
    short int fileDescriptor;

    operator std::string();
};

struct RecDataCloseDir{
    char operID;
    short int fileDescriptor;

    operator std::string();
};

struct fstatRetData
{
    char operID;
    char errorID;
	bool nfs_st_valid;		
    bool nfs_is_dir;
	int  nfs_st_size;	    
	int  nfs_st_atime;	    
	int  nfs_st_mtime;	

    operator std::string();	
};

struct RecDataOneLine{
    char operID;
    short int fileDescriptor;

    operator std::string();
};

struct DefRetIntSendData{
    char operID;
    char errorID;
    int retVal;

    operator std::string();
};



class Datagram
{
protected:
    size_t pos;
    size_t allDataSize;
    size_t bufSize;
    char* buffor;
    bool del;

public:
    Datagram(size_t bufSize);
    ~Datagram();
    
    size_t getPos() const;
    void setPos(size_t newPos);
    size_t getDataSize() const;
    size_t getBufSize() const;

    template<typename Data>
    void castBufferToStruct(Data& structure) const{
        memcpy(&structure, buffor, sizeof(Data));
    }
};

class Deserialize: public Datagram
{

public:
    Deserialize(const size_t bufSize);
    ~Deserialize() = default;
    void deserializePadding(const size_t size);
    int deserializeInt();
    void deserializeString(char* string, const size_t strSize);
    char deserializeChar();
    short int deserializeShortInt();

    template<typename Data>
    static ssize_t receiveStruct(Data& structure, const Client& client){
        ssize_t readFlag;

        if ((readFlag = read(client.getSocket(), &structure, sizeof(Data))) == -1)
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

    ssize_t receiveData(const Client& client);
};

class Serialize: public Datagram
{

public:
    Serialize(size_t bufSize);
    ~Serialize() = default;
    void serializePadding(const size_t size);
    void serializeInt(const int value);
    void serializeString(const char* string, const size_t size);
    void serializeChar(const char character);
    void serializeShortInt(const short int value);

    template<typename Data>
    static ssize_t sendStruct(Data& structure, const Client& client){
        ssize_t sendFlag;
        sendFlag = send(client.getSocket(), &structure, sizeof(Data), 0);

        if (sendFlag == -1)
        {
            std::cout << "Nie udalo sie wyslac wiadomosci " << std::endl;
        }
        else
        {
            std::cout << "Wyslano wiadomosc " << std::endl;
        }
        return sendFlag;
    }

    ssize_t sendData(const Client& client);
};