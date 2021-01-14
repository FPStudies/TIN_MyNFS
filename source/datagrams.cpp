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

const size_t Datagram::MAX_BUF_SIZE = UINT32_MAX - 1;

Datagram::Datagram(size_t bufSize) 
: pos(0), allDataSize(0), bufSize(bufSize), buffer(new char[bufSize]), del(true)
{}

Datagram::~Datagram() 
{
    if(del)
        delete[] (char*)buffer;
}

Deserialize::Deserialize(size_t bufSize)
: Datagram(bufSize)
{}

Serialize::Serialize(size_t bufSize)
: Datagram(bufSize)
{}

void Serialize::serializePadding(const size_t size){
    memset(buffer + pos, 0, size); // to not send trash data
    pos += size;
    allDataSize = std::max(pos, allDataSize);
}

void Serialize::serializeInt(const int value)
{
    memcpy(buffer + pos, &value, sizeof(int));
    pos += sizeof(int);
    allDataSize = std::max(pos, allDataSize);
}

void Serialize::serializeString(const char* string,const size_t size)
{
    if(size > bufSize)
        throw std::runtime_error("Buffer overflow.");
    memcpy(buffer + pos, string, size);
    pos += size;
    allDataSize = std::max(pos, allDataSize);
}

void Serialize::serializeChar(const char character)
{
    memcpy(buffer + pos, &character, sizeof(char));
    pos += sizeof(char);
    allDataSize = std::max(pos, allDataSize);
}  

void Serialize::serializeShortInt(const short int value)
{
    memcpy(buffer + pos, &value, sizeof(short int));
    pos += sizeof(short int);
    allDataSize = std::max(pos, allDataSize);
}

bool Deserialize::badLength(int length){
    if(length <= 0 || length > MAX_BUF_SIZE) return true;
    return false;
}

void Deserialize::deserializePadding(const size_t size){
    pos += size;
    allDataSize = std::max(pos, allDataSize);
}

int Deserialize::deserializeInt()
{
    int ret;
    memcpy(&ret, buffer + pos, sizeof(int));
    pos += sizeof(int);
    allDataSize = std::max(pos, allDataSize);
    return ret;
}

void Deserialize::deserializeString(char* string, const size_t strSize){
    if(strSize > bufSize)
        throw std::runtime_error("Buffer overflow.");
    memcpy(string, buffer + pos, strSize);
    pos += strSize;
    allDataSize = std::max(pos, allDataSize);
}

char Deserialize::deserializeChar(){
    char ret;
    memcpy(&ret, buffer + pos, sizeof(char));
    pos += sizeof(char);
    allDataSize = std::max(pos, allDataSize);
    return ret;
}

short int Deserialize::deserializeShortInt(){
    short int ret;
    memcpy(&ret, buffer + pos, sizeof(short int));
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

const char * const Datagram::getBuffer() const{
    return buffer;
}

ssize_t Deserialize::receiveData(const int socket, const int clientNumber, size_t size){
    ssize_t sum = 0;
    ssize_t readFlag = -1;
    
    while(sum < size){
        std::cout << "b read" + std::to_string(readFlag) + " " + std::to_string(pos) + " " +  std::to_string(bufSize) << std::endl;
        readFlag = read(socket, buffer + pos, bufSize);
        std::cout << "a read"  + std::to_string(readFlag) << std::endl;
        if(readFlag < 0){
            logInfo("Nie udalo sie odebrac. (" + std::to_string(clientNumber) + ")");
            return -1;
        }
        if(readFlag == 0){
            return sum;
        }

        pos += readFlag;
        sum += readFlag;
    }
    pos = 0;
    return sum;
}

ssize_t Serialize::sendData(const int socket, const int clientNumber)
{
    ssize_t writeFlag;
    writeFlag = send(socket, buffer, bufSize, 0);

    if (writeFlag == -1)
    {
        logInfo("Nie udalo sie wyslac wiadomosci (" + std::to_string(clientNumber) + ")");
    }
    else
    {
        std::cout << "send" << std::endl;
        logInfo("Wyslano wiadomosc (" + std::to_string(clientNumber) + ")");
    }
    return writeFlag;
}


OpenFileRecData::OpenFileRecData()
: operID(CHAR_MIN), padding1(CHAR_MIN), fileDescriptor(INT16_MIN), oflag(INT32_MIN), mode(INT32_MIN), pathLength(INT32_MIN)
{}

ReadFileRecData::ReadFileRecData()
: operID(CHAR_MIN), padding1(CHAR_MIN), fileDescriptor(INT16_MIN), length(INT32_MIN)
{}

DefRecIntData::DefRecIntData()
: operID(CHAR_MIN), padding1(CHAR_MIN), fileDescriptor(INT16_MIN), length(INT32_MIN)
{}

LseekRecData::LseekRecData()
: operID(CHAR_MIN), padding1(CHAR_MIN), fileDescriptor(INT16_MIN), whence(INT32_MIN), offset(INT32_MIN)
{}

CloseRecData::CloseRecData()
: operID(CHAR_MIN), padding1(CHAR_MIN), fileDescriptor(INT16_MIN)
{}

RecDataCloseDir::RecDataCloseDir()
: operID(CHAR_MIN), padding1(CHAR_MIN), fileDescriptor(INT16_MIN)
{}

fstatRetData::fstatRetData()
: operID(CHAR_MIN), errorID(CHAR_MIN), nfs_st_valid(false), nfs_is_dir(false), nfs_st_size(INT32_MIN), nfs_st_atime(INT32_MIN), nfs_st_mtime(INT32_MIN)
{}	

RecDataOneLine::RecDataOneLine()
: operID(CHAR_MIN), padding1(CHAR_MIN), fileDescriptor(INT16_MIN)
{}

DefRetIntSendData::DefRetIntSendData()
: operID(CHAR_MIN), errorID(CHAR_MIN), padding1(CHAR_MIN), padding2(CHAR_MIN), retVal(INT32_MIN)
{}

OpenFileRecData::operator std::string(){
    return "\noperID:\t" + std::to_string(operID) + "\nfileDescriptor:\t" + std::to_string(fileDescriptor) + "\noflag:\t" + std::to_string(oflag) + \
    "\nmode:\t" + std::to_string(mode) + "\npathLength:\t" + std::to_string(pathLength);
}

ReadFileRecData::operator std::string(){
    return "\noperID:\t" + std::to_string(operID) + "\nfileDescriptor:\t" + std::to_string(fileDescriptor) + "\nlength:\t" + std::to_string(length);
}

DefRecIntData::operator std::string(){
    return "\noperID:\t" + std::to_string(operID) + "\nfileDescriptor:\t" + std::to_string(fileDescriptor) + "\nlength:\t" + std::to_string(length);
}

LseekRecData::operator std::string(){
    return "\noperID:\t" + std::to_string(operID) + "\nfileDescriptor:\t" + std::to_string(fileDescriptor) + "\nwhence:\t" + std::to_string(whence) + \
    "\noffset:\t" + std::to_string(offset);
}

CloseRecData::operator std::string(){
    return "\noperID:\t" + std::to_string(operID) + "\nfileDescriptor:\t" + std::to_string(fileDescriptor);
}

RecDataCloseDir::operator std::string(){
    return "\noperID:\t" + std::to_string(operID) + "\nfileDescriptor:\t" + std::to_string(fileDescriptor);
}

fstatRetData::operator std::string(){
    return "\noperID:\t" + std::to_string(operID) + "\nerrorID:\t" + std::to_string(errorID) + "\nnfs_st_valid:\t" + std::to_string(nfs_st_valid) + \
    "\nnfs_st_size:\t" + std::to_string(nfs_st_size) + "\nnfs_st_atime:\t" + std::to_string(nfs_st_atime) + "\nnfs_st_mtime:\t" + std::to_string(nfs_st_mtime) +\
    "\nnfs_is_dir\t" + std::to_string(nfs_is_dir);
}

RecDataOneLine::operator std::string(){
    return "\noperID:\t" + std::to_string(operID) + "\nfileDescriptor:\t" + std::to_string(fileDescriptor);
}

DefRetIntSendData::operator std::string(){
    return "\noperID:\t" + std::to_string(operID) + "\nerrorID:\t" + std::to_string(errorID) + "\nretVal:\t" + std::to_string(retVal);
}