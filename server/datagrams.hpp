#pragma once
#include <iostream>

class Datagrams
{
public:
    Datagrams();
    char * serializeInt(char * buffor, int value, int size);
    char * serializeChar(char * buffor, char * string, int size);
    int deserializeInt(char* buffor, int size);
};