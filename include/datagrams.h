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
#include "LogUtils.h"

struct OpenFileRecData{
    char operID;
    short int fileDescriptor;
    int oflag;
    int mode;
    int pathLength;

    OpenFileRecData();
};

struct ReadFileRecData{
    char operID;
    short int fileDescriptor;
    int length;

    ReadFileRecData();
};

struct DefRecIntData{
    char operID;
    short int fileDescriptor;
    int length;

    DefRecIntData();
};


struct LseekRecData{
    char operID;
    short int fileDescriptor;
    int whence;
    int offset;

    LseekRecData();
};

struct CloseRecData{
    char operID;
    short int fileDescriptor;

    CloseRecData();
};

struct RecDataCloseDir{
    char operID;
    short int fileDescriptor;

    RecDataCloseDir();
};


struct fstatRetData
{
    char operID;
    char errorID;
	bool nfs_st_valid;		
	int  nfs_st_size;	    
	int  nfs_st_atime;	    
	int  nfs_st_mtime;	

    fstatRetData();
};

struct RecDataOneLine{
    char operID;
    short int fileDescriptor;

    RecDataOneLine();
};

struct DefRetIntSendData{
    char operID;
    char errorID;
    int retVal;

    DefRetIntSendData();
};

class Datagram
{
protected:
    size_t pos;
    size_t allDataSize;
    size_t bufSize;
    char* buffer;
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
        memcpy(&structure, buffer, sizeof(Data));
    }

    const char * const getBuffer() const;
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
    static ssize_t receiveStruct(Data& structure, const int socket, const int clientNumber){
        ssize_t readFlag;
    
        if ((readFlag = read(socket, &structure, sizeof(Data))) == -1)
        {
            logInfo("Nie udalo sie odebrac stuktury. (" + std::to_string(clientNumber) + ")");
            return -1;
        }
        else if (readFlag == 0)
        {
            logInfo("Koniec polaczenia z klientem " + std::to_string(clientNumber) + ")");
            return 0;
        }
        
        return readFlag;
    }

    ssize_t receiveData(const int socket, const int clientNumber);
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
    static ssize_t sendStruct(Data& structure, const int socket, const int clientNumber){
        ssize_t writeFlag;
        writeFlag = send(socket, &structure, sizeof(Data), 0);

        if (writeFlag == -1)
        {
            logInfo("Nie udalo sie wyslac stuktury (" + std::to_string(clientNumber) + ")");
        }
        else
        {
            logInfo("Wyslano wiadomosc (" + std::to_string(clientNumber) + ")");
        }
        return writeFlag;
    }

    ssize_t sendData(const int socket, const int clientNumber);
};