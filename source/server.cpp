/**
 * @file server.cpp
 * @author Maciej Adamski, Mateusz Kordowski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

class ClientHandler;

#include "server.h"

const std::string Server::MAIN_LOG_NAME("mainThread");
int Server::socket_id = -1;

/**
 * @author Mateusz Kordowski
 * 
 */
void INThandler(int sig)
{
    if(Server::socket_id != -1)
        close(Server::socket_id);
}

/**
 * @author Mateusz Kordowski
 * 
 */
ThreadStruct::ThreadStruct(int sock, const int& threadID, IDGenMonitor& gen)
: sock(sock), threadIDGen(gen), threadID(threadID)
{}
#ifdef ENABLE_LOGS
std::shared_ptr<spdlog::logger> createLogger(int threadID, int sock, int clientNums){
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto logger = spdlog::get(logName(threadID)); // sprawdzam czy istnieje

    // usuńcie środkową linię aby nie podawało daty do nazwy pliku.
    if(logger == nullptr){
        logger = spdlog::basic_logger_mt<spdlog::async_factory>(logName(threadID), "logs/thread_" + std::to_string(threadID) \
        /*+ "_" + std::string(std::ctime(&currentTime))*/ \
        + ".txt");
    }
    
    logger->info("Utworzono watek klienta nr " + std::to_string(clientNums) + ", socket " + std::to_string(sock));
    return logger;
}
#endif

/**
 * @author Mateusz Kordowski, Maciej Adamski
 * 
 */
void *clientThread(void *arg)
{
    static int clientNums = 1;
    ThreadStruct* threadStruct = static_cast<ThreadStruct*>(arg);

    auto& tid = ThreadID::getInstance();
    tid.add(threadStruct->threadID);

    #ifdef ENABLE_LOGS
    auto logger = createLogger(threadStruct->threadID, threadStruct->sock, clientNums);
    #endif

    ClientHandler handler(clientNums++, threadStruct->sock);

    handler.run();

	close(threadStruct->sock);
    #ifdef ENABLE_LOGS
    logger->info("Zamknieto socket " + std::to_string(threadStruct->sock) + " Thread ID: " + std::to_string(threadStruct->threadID));
    #endif
    threadStruct->threadIDGen.dispose(threadStruct->threadID);
    delete threadStruct;
	pthread_exit(NULL);
}


/**
 * @author Mateusz Kordowski, Maciej Adamski
 * 
 */
void Server::run()
{
    signal(SIGINT, INThandler);

    #ifdef ENABLE_LOGS
    spdlog::flush_on(spdlog::level::info);  // 
    spdlog::flush_on(spdlog::level::debug);  // aby podczas ctrl ^ c zapisało logi do plików. 
    auto tmp = spdlog::basic_logger_mt(Server::MAIN_LOG_NAME, "logs/mainThread.txt");

    // specjalny kod w postaci -1 oznacza główny wątek programu.
    ThreadID::getInstance().add(-1); // dla umożliwienia logowania z innych funkcji, co nie korzystają z MAIN_LOG_NAME.
    tmp->info("Main thread id: " + std::to_string(SERVER_THREAD_ID));
    auto logger = createLogger(SERVER_THREAD_ID, -1, -1); // trzeba trzymać inaczej zwolni zasoby.
    #endif
    
    createConnectSocket();
    bindPort();

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bindSocket();
    setListen();
    connectLoop();
}

/**
 * @author Maciej Adamski
 * 
 */
void Server::createConnectSocket()
{
    connectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectSocket == 0)
	{
        logInfoKnownThreadName(Server::MAIN_LOG_NAME, "Nie utworzono socketu");
		exit(1);
	}
    Server::socket_id = connectSocket;
}

/**
 * @author Maciej Adamski
 * 
 */
void Server::bindPort()
{
    int portOption = 1;
    if (setsockopt(connectSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &portOption, sizeof(portOption)))
	{
        logInfoKnownThreadName(Server::MAIN_LOG_NAME, "Nie udalo sie ustawic portu");
		exit(1);
	}
}

/**
 * @author Maciej Adamski
 * 
 */
void Server::bindSocket()
{
    if (bind(connectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
        logInfoKnownThreadName(Server::MAIN_LOG_NAME, "Nie dowiazano socketu");
		exit(1);
	}
}

/**
 * @author Maciej Adamski
 * 
 */
void Server::setListen()
{
    if (listen(connectSocket, MAX_USER) == -1)
    {
        logInfoKnownThreadName(Server::MAIN_LOG_NAME, "Blad nasluchiwania");
        exit(1);
    }
}

/**
 * @author Mateusz Kordowski
 * 
 */
void Server::connectLoop()
{
    const int max = MAX_USER;
    pthread_t tid[max];
    IDGenMonitor generator(0, max);
    std::set<int> usedThreadID;

    while(true){
        int sock;
        auto threadID = generator.get();
        sock = createUserSocket();
        if(sock == -1){
            break;
        }
        ThreadStruct* theradArg = new ThreadStruct(sock, threadID, generator);
        logInfoKnownThreadName(Server::MAIN_LOG_NAME, "Pojawil sie nowy klient. Socket nr " + std::to_string(sock));

        logInfoKnownThreadName(Server::MAIN_LOG_NAME, "Gen size: " + std::to_string(generator.size()));
        if (threadID == INT32_MIN || pthread_create(&tid[threadID], NULL, clientThread, theradArg) != 0)
        {
            logInfoKnownThreadName(Server::MAIN_LOG_NAME, "Nie udalo sie utworzyc watku");
            delete theradArg;
            break;
        }
        usedThreadID.insert(threadID);
    }

    for(auto it : usedThreadID){
        pthread_join(tid[it], NULL);
    }
}

/**
 * @author Maciej Adamski
 * 
 */
int Server::createUserSocket()
{
    int sock;
    logInfoKnownThreadName(Server::MAIN_LOG_NAME, "Oczekiwanie na klienta");
    if ((sock = accept(connectSocket, (struct sockaddr*)0, (socklen_t*)0)) == -1)
    {
        logInfoKnownThreadName(Server::MAIN_LOG_NAME, "Blad akcepta");
        return -1;
    }

    return sock;
}


