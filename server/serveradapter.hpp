#pragma once

#include "clienthandler.hpp"

class ServerAdapter
{
public:
    ClientHandler * clientHandler;
    
    ServerAdapter()
    {
        clientHandler = new ClientHandler(2, 3);
        clientHandler->run();
    }
};