#pragma once
#include <iostream>
class Datagrams
{
public:
    Datagrams();
    char * serializeInt(char * buffor, int value, int size);
    int deserializeInt(char* buffor, int size);
    char * deserializeChar(char * buffor, int size);
};