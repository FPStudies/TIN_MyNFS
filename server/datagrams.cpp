#include "datagrams.hpp"

Datagrams::Datagrams() 
{}

char * Datagrams::serializeInt(char * buffor, int value, int size)
{
    for (int i = size; i > 0; i--)
    {
        buffor[size - i] = value >> (8 * (i -1));
    }

    return buffor + size;
}

char * Datagrams::serializeChar(char * buffor, char * string, int size)
{
    for (int i = 0; i < size; i++)
    {
        buffor[i] = string[i];
    }

    return buffor + size;
}
int Datagrams::deserializeInt(char* buffor, int size)
{
    int val = 0;
    for (int i = size; i > 0; i--)
    {
        val |= (buffor[size - i] << (8 * (i -1)));
    }
    return val;
}