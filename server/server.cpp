/**
 * @file server.cpp
 * @author Maciej Adamski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include "server.hpp"
#include "clienthandler.hpp"

class ClientHandler;

void *clientThread(void *arg)
{
    static int clientNums = 1;
    int sock = *((int *) arg);
    std::cout << "Utworzono watek klienta nr " << clientNums << ", socket " << sock << std::endl;
    ClientHandler handler(clientNums++, sock);

    handler.run();

	close(sock);
    std::cout << "Zamknieto socket " << sock << std::endl;
	pthread_exit(NULL);
}

void Server::run()
{
    createConnectSocket();
    bindPort();

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bindSocket();
    setListen();
    connectLoop();
}

void Server::createConnectSocket()
{
    connectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectSocket == 0)
	{
		std::cout << "Nie utworzono socketu" << std::endl;
		exit(1);
	}
}

void Server::bindPort()
{
    int portOption = 1;
    if (setsockopt(connectSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &portOption, sizeof(portOption)))
	{
		std::cout << "Nie udalo sie ustawic portu" << std::endl;
		exit(1);
	}
}

void Server::bindSocket()
{
    if (bind(connectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
		std::cout << "Nie dowiazano socketu" << std::endl;
		exit(1);
	}
}

void Server::setListen()
{
    if (listen(connectSocket, MAX_USER) == -1)
    {
        std::cout << "Blad nasluchiwania" << std::endl;
        exit(1);
    }
}

void Server::connectLoop()
{
    pthread_t tid[MAX_USER];

    int sock;
    for (int i = 0; i < MAX_USER; i++)
    {
        sock = createUserSocket();

        std::cout << "Pojawil sie nowy klient. Socket nr " << sock << std::endl;

        if (pthread_create(&tid[i], NULL, clientThread, &sock) != 0)
        {
            std::cout << "Nie udalo sie utworzyc watku" << std::endl;
            exit(1);
        }

    }

    for (int i = 0; i < MAX_USER; i++)
	{
		pthread_join(tid[i], NULL);
	}
}

int Server::createUserSocket()
{
    int sock;
    std::cout << "Oczekiwanie na klienta" << std::endl;
    if ((sock = accept(connectSocket, (struct sockaddr*)0, (socklen_t*)0)) == -1)
    {
        std::cout << "Blad akcepta" << std::endl;
        exit(1);
    }

    return sock;
}


