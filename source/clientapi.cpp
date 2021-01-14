/**
 * @file clientapi.cpp
 * @author Mateusz Kordowski, Maciej Adamski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "clientapi.h"

int mynfs_errno = 0;

bool isSockGood(int socket_fd){
    logStart();
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);
    std::cout << "Socket, retval: " << socket_fd << " " << retval << " " << error << std::endl;
    if (retval != 0) {
        logError("Getting socket error code: " + std::string(strerror(retval)) );
        /* there was a problem getting the error code */
        //fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        return false;
    }

    if (error != 0) {
        /* socket has a non zero error status */
        logError("Socket returned error: " + std::string(strerror(error)));
        //fprintf(stderr, "socket error: %s\n", strerror(error));
        return false;
    }
    logEndCustom("Passed");
    return true;
}



ClientApi::ClientApi()
{

}

ClientApi::~ClientApi()
{
    //clean *Clients
    for (auto const& it : clients)
    {
        delete it.second;
    }
}

int ClientApi::mynfs_open(char * host, char* path, int oflag, int mode)
{
    logStart();
    if(path == NULL || path == nullptr){
        setErrno((int)MyNFS_ERRORS::enoent);
        logError("ENOENT");
        return -1;
    }

    if(path[0] != '/')
    {
        setErrno((int)MyNFS_ERRORS::enotdir);
        logError("Sciezka nie zaczyna sie od znaku '/'");
        return -1;
    }

    int pathLength = strlen(path) + 1;

    if (pathLength > 4096)
    {
        setErrno((int)MyNFS_ERRORS::enametoolong);
        logError("Za dluga sciezka");
        return -1;
    }

    Client * client = nullptr;
    int pos = 0;

    // Szukamy czy mamy już takie polaczenie
    for (auto const& it : clients)
    {
        if (strcmp(it.second->getAddress(), host) == 0)
        {
            logCustom("Znaleziono socket");
            client = it.second;
            pos = it.first;
            break;
        }
    } 

    char trashbuf[16];

    if(client != nullptr){
        if (recv(client->getSocket(),trashbuf,1, MSG_PEEK | MSG_DONTWAIT) == 0)
        {
            logError("Socket is not valid");
            delete client;
            client = nullptr;
            clients.erase(pos);
        }
    }

    if (client == nullptr)
    {
        logCustom("Opening new socket");
        // otwiweramy nowe
        client = new Client(host);
        client->startConnection();
    }
    
    OpenFileRecData sendData;
    sendData.operID = static_cast<char>(ApiIDS::OPEN);
    sendData.fileDescriptor = 0;
    sendData.oflag = oflag;
    sendData.mode = mode;
    sendData.pathLength = pathLength;
    logSendStructMessage(sendData, "");
    auto tmp = Serialize::sendStruct(sendData, *client);
    if(tmp == -1){
        //int error_code;
        //int error_code_size = sizeof(error_code);
        //getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);

        logError("Socket is not valid\n");
        setErrno((int)MyNFS_ERRORS::epipe); 
        return -1;
    }
    logCustom("Struct send");

    // TODO odebrać potwierdzenie
    DefRetIntSendData recOk;
    logCustom("Waiting to receive struct");
    Deserialize::receiveStruct(recOk, *client);
    logReceiveStructMessage(recOk, "");
    if(recOk.retVal == -1)
    {
        logError("Server encountered an error");
        setErrno(recOk.errorID); 
        return -1;
    }
    
    Serialize sendPath(pathLength);
    sendPath.serializeString(path, pathLength);
    logSendStringMessage(std::string(path), "");
    sendPath.sendData(*client);
    logCustom("String send");
    DefRetIntSendData rec;
    logCustom("Waiting to receive struct");
    if (( Deserialize::receiveStruct(rec, *client)) == -1 )
    {
        logError("did not receive data");
        setErrno((int)MyNFS_ERRORS::eserv);
    }
    logReceiveStructMessage(rec, "");
    if (rec.retVal == -1)
    {
        logError("Server encountered an error, returned -1");
        setErrno(rec.errorID);
    }
    // std::cout << "Zwrocono FD: " << rec.retVal << ", error: " << static_cast<int>(rec.errorID) << std::endl;

    clients.insert(std::pair<int, Client*> (rec.retVal, client));
    logEndCustom("Pass");
    return rec.retVal;
}

int ClientApi::mynfs_read(int mynfs_fd, char * buf, int len)
{
    logStart();
    if (!clientExist(mynfs_fd))
    {
        logError("Bad file descriptor");
        setErrno((int)MyNFS_ERRORS::ebadf);
        return -1;
    }

    Client * client = clients[mynfs_fd];

    DefRecIntData sendData;
    sendData.operID = static_cast<char>(ApiIDS::READ);
    sendData.fileDescriptor = mynfs_fd;
    sendData.length = len;
    logSendStructMessage(sendData, "");
    Serialize::sendStruct(sendData, *client);
    logCustom("Struct send");

    DefRetIntSendData recData;
    logCustom("Waiting to receive struct");
    Deserialize::receiveStruct(recData, *client);
    logReceiveStructMessage(recData, "");
    if(recData.retVal <= 0)
    {
        logError("did not receive data");
        setErrno((int)MyNFS_ERRORS::eserv);
        return recData.retVal;
    };


    if (recData.errorID == 0)
    {
        RecDataOneLine recOk;
        recOk.fileDescriptor = mynfs_fd;
        recOk.operID = static_cast<char>(ApiIDS::READ);
        Serialize::sendStruct(recOk, *client);
        Deserialize recString(len); // TODO sprawdzić czy dla dużych wartości działa
        logCustom("Waiting to receive string");
        recString.receiveData(*client, len);
        recString.deserializeString(buf, recData.retVal);
        logReceiveStringMessage(std::string(buf), "");
        logEndCustom("Pass");
        return recData.retVal;
    }

    logError("server encountered an error");
    setErrno(recData.errorID);
    return -1;
}

int ClientApi::mynfs_write(int mynfs_fd, const char * buf, int len)
{
    logStart();
    if (!clientExist(mynfs_fd))
    {
        logError("Bad file descriptor");
        setErrno((int)MyNFS_ERRORS::ebadf);
        return -1;
    }

    Client * client = clients[mynfs_fd];

    DefRecIntData sendData;
    sendData.operID = static_cast<char>(ApiIDS::WRITE);
    sendData.fileDescriptor = mynfs_fd;
    sendData.length = len;
    logSendStructMessage(sendData, "");
    Serialize::sendStruct(sendData, *client);// wysylamy naglowek
    logCustom("Struct send");
    DefRetIntSendData recOk;
    Deserialize::receiveStruct(recOk, *client);
    logReceiveStructMessage(recOk, "");
    if (recOk.retVal == -1)
    {
        logError("Server encountered an error");
        setErrno(recOk.errorID);
        return -1;
    }

    Serialize sendStr(len);
    sendStr.serializeString(buf, len);
    logSendStringMessage( std::string(buf), "");
    sendStr.sendData(*client);// wysylamy dane do zapisu
    logCustom("String send");

    DefRetIntSendData recData;
    logCustom("Waiting to receive struct");
    Deserialize::receiveStruct(recData, *client);
    logReceiveStructMessage(recData, "");

    if (recData.retVal == -1)
    {
        setErrno(recData.errorID);
        logError("Server encountered an error, returned -1");
        return -1;
    }

    logEndCustom("Pass");
    return recData.retVal;
}

int ClientApi::mynfs_lseek(int mynfs_fd, int whence, int offset)
{
    logStart();
    if (!clientExist(mynfs_fd))
    {
        logError("Bad file descriptor");
        setErrno((int)MyNFS_ERRORS::ebadf);
        logError("Client does not exist. Fail");
        return -1;
    }

    Client * client = clients[mynfs_fd];
    LseekRecData sendData;
    sendData.operID = static_cast<char>(ApiIDS::LSEEK);
    sendData.fileDescriptor = mynfs_fd;
    sendData.whence = whence;
    sendData.offset = offset;
    logSendStructMessage(sendData, "");
    Serialize::sendStruct(sendData, *client);
    logCustom("Struct send");

    DefRetIntSendData recData;
    logCustom("Waiting to receive struct");
    Deserialize::receiveStruct(recData, *client);
    logReceiveStructMessage(recData, "");
    if (recData.retVal == -1)
    {
        setErrno(recData.errorID);
        logError("Server encountered an error, returned -1");
        return -1;
    }
    std::cout << "Zwrocono ret: " << recData.retVal << ", error: " << static_cast<int>(recData.errorID) << std::endl;

    logEndCustom("Pass");
    return recData.retVal;
}

int ClientApi::mynfs_close(int mynfs_fd)
{
    logStart();
    if (!clientExist(mynfs_fd))
    {
        logError("Bad file descriptor");
        setErrno((int)MyNFS_ERRORS::ebadf);
        return -1;
    }
    
    Client * client = clients[mynfs_fd];

    RecDataOneLine sendData;
    sendData.operID = static_cast<char>(ApiIDS::CLOSE);
    sendData.fileDescriptor = mynfs_fd;
    logSendStructMessage(sendData, "");
    Serialize::sendStruct(sendData, *client);
    logCustom("Struct send");

    DefRetIntSendData recData;
    logCustom("Waiting to receive struct");
    Deserialize::receiveStruct(recData, *client);
    if (recData.retVal == -1)
    {
        logError("Server encountered an error, returned -1");
        setErrno(recData.errorID);
    }
    logReceiveStructMessage(recData, "");
    //std::cout << "Zwrocono ret: " << recData.retVal << ", error: " << static_cast<int>(recData.errorID) << std::endl;

    logEndCustom("Pass");
    usleep(10000);

    return recData.retVal;
}

int ClientApi::mynfs_unlink(char *host, char *path)
{
    logStart();

    if(path == NULL || path == nullptr){
        setErrno((int)MyNFS_ERRORS::enoent);
        logError("ENOENT");
        return -1;
    }

    int pathLength = strlen(path) + 1;

    if (pathLength > 4096)
    {
        logError("Za dluga sciezka");
        setErrno((int)MyNFS_ERRORS::enametoolong);
        return -1;
    }

    Client * client = nullptr;

    for (auto const& it : clients)
    {
        if (strcmp(it.second->getAddress(), host) == 0)
        {
            logCustom("Socket found");
            client = it.second;
            break;
        }
    } 

    if(client != nullptr){
        if (recv(client->getSocket(),NULL,1, MSG_PEEK | MSG_DONTWAIT) == 0)
        {
            logError("Socket is not valid");
            delete client;
            client = nullptr;
        }
    }

    if (client == nullptr)
    {
        logCustom("Opening new socket");
        // otwiweramy nowe
        client = new Client(host);
        client->startConnection();
    }

    DefRecIntData sendData;
    sendData.operID = static_cast<char>(ApiIDS::UNLINK);
    sendData.fileDescriptor = 0;
    sendData.length = pathLength;
    logSendStructMessage(sendData, "");
    auto tmp = Serialize::sendStruct(sendData, *client); // send
    if(tmp == -1){
        //int error_code;
        //int error_code_size = sizeof(error_code);
        //getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);

        logError("Socket is not valid\n");
        setErrno((int)MyNFS_ERRORS::epipe); 
        return -1;
    }
    logCustom("Struct send");
    // TODO odebrać potwierdzenie
    DefRetIntSendData recOk;
    logCustom("Waiting to receive struct");
    if (Deserialize::receiveStruct(recOk, *client) == -1)
    {
        logError("Receive failed");
        setErrno(-1);
        return -1;
    }
    logReceiveStructMessage(recOk, ""); // rec
    if(recOk.retVal == -1)
    {
        logError("Server encountered an error");
        setErrno(recOk.errorID); 
        return -1;
    }

    Serialize sendPath(pathLength);
    sendPath.serializeString(path, pathLength);
    logSendStringMessage(std::string(path), "");
    sendPath.sendData(*client); // send
    logCustom("String send");
    DefRetIntSendData rec;
    if (( Deserialize::receiveStruct(rec, *client)) == -1 )
    {
        logError("did not receive data");
        setErrno((int)MyNFS_ERRORS::eserv);
        return -1;
    }
    if (rec.retVal == -1)
    {
        logError("Server encountered an error, returned -1");
        setErrno(rec.errorID);
        return -1;
    }


    logEndCustom("Pass");
    return rec.retVal;
}


int ClientApi::mynfs_closedir(int dirfd)
{
    logStart();
    if (!clientExist(dirfd))
    {
        logError("Bad file descriptor");
        setErrno((int)MyNFS_ERRORS::ebadf);
        return -1;
    }

    Client * client = clients[dirfd];
    RecDataOneLine sendData;
    sendData.operID = static_cast<char>(ApiIDS::CLOSEDIR);
    sendData.fileDescriptor = dirfd;
    logSendStructMessage(sendData, "");
    Serialize::sendStruct(sendData, *client);
    logCustom("Struct send");

    DefRetIntSendData recData;
    logCustom("Waiting to receive struct");
    Deserialize::receiveStruct(recData, *client);
    logReceiveStructMessage(recData, "");
    if (recData.retVal == -1)
    {
        logError("Server encountered an error, returned -1");
        setErrno(recData.errorID);
    }
    std::cout << "Zwrocono ret: " << recData.retVal << ", error: " << static_cast<int>(recData.errorID) << std::endl;
    usleep(10000);

    return recData.retVal; 
}

char * ClientApi::mynfs_readdir(int dirfd)
{
    logStart();
    if (!clientExist(dirfd))
    {
        logError("Bad file descriptor");
        setErrno((int)MyNFS_ERRORS::ebadf);
        return NULL;
    }

    Client * client = clients[dirfd];
    RecDataOneLine sendData;
    sendData.operID = static_cast<char>(ApiIDS::READDIR);
    sendData.fileDescriptor = dirfd;
    logSendStructMessage(sendData, "");
    Serialize::sendStruct(sendData, *client);
    logCustom("Struct send");

    DefRetIntSendData recData;
    logCustom("Waiting to receive struct");
    Deserialize::receiveStruct(recData, *client);
    logReceiveStructMessage(recData, "");
    // TODO wysłanie potwierdzenia wysłania takiej ilości bajtów
    
    DefRetIntSendData recOk;
    recOk.retVal = recData.retVal;
    recOk.operID = recData.operID;
    recOk.errorID = 0;
    logSendStructMessage(recOk, "");
    Serialize::sendStruct(recOk, *client);
    logCustom("Struct send");

    if (recData.retVal == 0)
    {
        return new char[1]();
    }
    if (recData.retVal > 0)
    {
    
        Deserialize recStr(recData.retVal);
        logCustom("Waiting to receive string");
        recStr.receiveData(*client, recData.retVal);
        
        char * dirString = new char[recData.retVal]();
        logReceiveStringMessage(std::string(dirString), "");
        recStr.deserializeString(dirString, recData.retVal);

        // dirString - zwrócona zawartość katalogu
        return dirString;
    }
    else
    {
        logError("Server encountered an error, returned negative");
        setErrno(recData.errorID);
        return NULL;
    }
}

int ClientApi::mynfs_opendir(char *host, char *path)
{
    logStart();
    // return mynfs_open(host, path, O_DIRECTORY, 0);

    logStart();
    if(path == NULL || path == nullptr){
        setErrno((int)MyNFS_ERRORS::enoent);
        logError("ENOENT");
        return -1;
    }

    if(path[0] != '/')
    {
        logError("Sciezka nie zaczyna sie od znaku '/'");
        setErrno((int)MyNFS_ERRORS::enotdir);
        return -1;
    }

    int pathLength = strlen(path) + 1;

    if (pathLength > 4096)
    {
        logError("Za dluga sciezka");
        setErrno((int)MyNFS_ERRORS::enametoolong);
        return -1;
    }

    Client * client = nullptr;
    int pos=0;

    // Szukamy czy mamy już takie polaczenie
    for (auto const& it : clients)
    {
        if (strcmp(it.second->getAddress(), host) == 0)
        {
            logCustom("Socket found");
            client = it.second;
            pos = it.first;
            break;
        }
    } 

    char trashbuf[16];

    if(client != nullptr){
        if (recv(client->getSocket(),trashbuf,1, MSG_PEEK | MSG_DONTWAIT) == 0)
        {
            logError("Socket is not valid");
            delete client;
            client = nullptr;
            clients.erase(pos);
        }
    }

    if (client == nullptr)
    {
        logCustom("Opening new socket");
        // otwiweramy nowe
        client = new Client(host);
        client->startConnection();
    }
    
    DefRecIntData sendData;
    sendData.operID = static_cast<char>(ApiIDS::OPENDIR);
    sendData.fileDescriptor = 0;
    sendData.length = pathLength;
    logSendStructMessage(sendData, "");
    auto tmp = Serialize::sendStruct(sendData, *client); // send
    if(tmp == -1){
        //int error_code;
        //int error_code_size = sizeof(error_code);
        //getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);

        logError("Socket is not valid\n");
        setErrno((int)MyNFS_ERRORS::epipe); 
        return -1;
    }
    logCustom("Struct send");
    // TODO odebrać potwierdzenie
    DefRetIntSendData recOk;
    logCustom("Waiting to receive struct");
    if (Deserialize::receiveStruct(recOk, *client) == -1)
    {
        logError("Receive failed");
        setErrno(-1);
        return -1;
    }
    logReceiveStructMessage(recOk, ""); // rec
    if(recOk.retVal == -1)
    {
        logError("Server encountered an error");
        setErrno(recOk.errorID); 
        return -1;
    }

    Serialize sendPath(pathLength);
    sendPath.serializeString(path, pathLength);
    logSendStringMessage(std::string(path), "");
    sendPath.sendData(*client); // send
    logCustom("String send");
    DefRetIntSendData rec;
    if (( Deserialize::receiveStruct(rec, *client)) == -1 )
    {
        logError("did not receive data");
        setErrno((int)MyNFS_ERRORS::eserv);
        return -1;
    }
    if (rec.retVal == -1)
    {
        logError("Server encountered an error, returned -1");
        setErrno(rec.errorID);
        return -1;
    }
    // std::cout << "Zwrocono FD: " << rec.retVal << ", error: " << static_cast<int>(rec.errorID) << std::endl;

    clients.insert(std::pair<int, Client*> (rec.retVal, client));
    logEndCustom("Pass");
    return rec.retVal;
}

mynfs_stat ClientApi::mynfs_fstat(int mynfs_fd)
{
    logStart();
    mynfs_stat mstat = {};
    if (!clientExist(mynfs_fd))
    {
        logError("Bad file descriptor");
        setErrno((int)MyNFS_ERRORS::ebadf);
        mstat.nfs_st_valid = false;
        return mstat;
    }

    Client * client = clients[mynfs_fd];
    RecDataOneLine sendData;
    sendData.operID = static_cast<char>(ApiIDS::FSTAT);
    sendData.fileDescriptor = mynfs_fd;
    logSendStructMessage(sendData, "");
    Serialize::sendStruct(sendData, *client);
    logCustom("Struct send");

    fstatRetData recData;
    logCustom("Waiting to receive struct");
    Deserialize::receiveStruct(recData, *client);
    logReceiveStructMessage(recData, "");;
    if(recData.nfs_st_valid == false)
    {
        setErrno(recData.errorID);
    }

    mstat.nfs_st_valid = recData.nfs_st_valid;
    mstat.nfs_st_size = recData.nfs_st_size;
    mstat.nfs_st_atime = recData.nfs_st_atime;
    mstat.nfs_st_mtime = recData.nfs_st_mtime;
    mstat.nfs_st_atime = recData.nfs_is_dir;

    return mstat;
}

void ClientApi::setErrno(int errorID)
{
    mynfs_errno = errorID;
}

bool ClientApi::clientExist(int fd)
{
    return !(clients.find(fd) == clients.end());
}