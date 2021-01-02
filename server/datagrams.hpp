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

    char * serializeChar(char * buffor, char * string, int size)
    {
        for (int i = 0; i < size; i++)
        {
            buffor[i] = string[i];
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
};