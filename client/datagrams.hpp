#pragma once
#include <iostream>
#include <cstring>

class Datagrams
{
public:
Datagrams() {};

    void serializePadding(void* buffor, size_t size, size_t& pos){
        memset(buffor + pos, 0, size); // to not send trash data
        pos += size;
    }

    void serializeInt(void* buffor, int value, size_t& pos)
    {
        memcpy(buffor + pos, &value, sizeof(int));
        pos += sizeof(int);
    }

    void serializeString(void* buffor, char* string, size_t size, size_t& pos)
    {
        memcpy(buffor + pos, string, size);
        pos += size;
    }

    void serializeChar(void* buffor, char character, size_t& pos)
    {
        memcpy(buffor + pos, &character, sizeof(char));
        pos += sizeof(char);
    }  

    void serializeShortInt(void* buffor, short int value, size_t& pos)
    {
        memcpy(buffor + pos, &value, sizeof(short int));
        pos += sizeof(short int);
    }

    void deserializePadding(void* buffor, size_t size, size_t& pos){
        pos += size;
    }

    int deserializeInt(void* buffor, size_t& pos)
    {
        int ret;
        memcpy(&ret, buffor + pos, sizeof(int));
        pos += sizeof(int);
        return ret;
    }

    void deserializeString(void* buffor, char* string, size_t strSize, size_t& pos){
        memcpy(string, buffor + pos, strSize);
        pos += strSize;
    }

    char deserializeChar(void* buffor, size_t& pos){
        char ret;
        memcpy(&ret, buffor + pos, sizeof(char));
        pos += sizeof(char);
        return ret;
    }

    short int deserializeShortInt(void* buffor, size_t& pos){
        short int ret;
        memcpy(&ret, buffor + pos, sizeof(short int));
        pos += sizeof(short int);
        return ret;
    }
};