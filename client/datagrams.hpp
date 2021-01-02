#pragma once
#include <iostream>
class Datagrams
{
public:
Datagrams() {};
    char * serializeInt(char * buffor, int value, int size)
    {
        for (int i = size; i > 0; i--)
        {
            buffor[size - i] = value >> (8 * (i -1));
        }

        return buffor + size;
    }

    int deserializeInt(char* buffor, int size)
    {
        int val = 0;
        for (int i = size; i > 0; i--)
        {
            val |= buffor[size - i] << (8 * (i -1));
        }

        return val;
    }

    char * deserializeChar(char * buffor, int size)
    {
        char * newChar = new char[size];
        for (int i = 0; i < size; i++)
        {
            newChar[i] = buffor[i];
        }
        return newChar;
    }
};