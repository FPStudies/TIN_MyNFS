/**
 * @file clienthandler.cpp
 * @author Mateusz Kordowski, Maciej Adamski
 * @brief 
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "clienthandler.h"

ClientHandler::ClientHandler(int clientNum, int sock) : clientNum(clientNum), sock(sock)
{

}

void ClientHandler::run()
{
    bool run = true;
    int readFlag;
    ApiIDS id;
    FDManager fdManager;
    IDGen idGen;
    logInfo("Polaczono klienta nr " + std::to_string(clientNum) + ". Socket " + std::to_string(sock));
    while(run)
    {
        Deserialize data(16);
        readFlag = data.receiveData(sock, clientNum);
        if (readFlag <= 0)
            run = false;
        else
        {
            id = static_cast<ApiIDS>(data.deserializeChar());
            logInfo("ID operacji: " + std::to_string(static_cast<int>(id)));
            switch (id)
            {
                case ApiIDS::OPEN:
                    openFile(data, fdManager, idGen);
                    break;
                case ApiIDS::READ:
                    readFile(data, fdManager);
                    break;
                case ApiIDS::WRITE:
                    writeFile(data, fdManager);
                    break;
                case ApiIDS::LSEEK:
                    lseekFile(data, fdManager);
                    break;
                case ApiIDS::CLOSE:
                    closeFile(data, fdManager, run);
                    break;
                case ApiIDS::CLOSEDIR:
                    closeDir(data, fdManager, run);
                    break;
                case ApiIDS::READDIR:
                    readDir(data, fdManager);
                    break;
                case ApiIDS::FSTAT:
                    fstatFile(data, fdManager);
                    break;
                case ApiIDS::OPENDIR:
                    openDir(data, fdManager, idGen);
                    break;        
                default:
                    logInfo("nieznana operacja"); // TODO      
            }
        }
    }
}

void ClientHandler::openFile(Deserialize& data, FDManager& manager, IDGen& gen)
{
    API api;
    OpenFileRecData received;
    DefRetIntSendData ret;
    data.castBufferToStruct(received);
    logReceiveStructMessage(received, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
 
    // DefRecIntData recOk;
    // recOk.operID = received.operID;
    // recOk.fileDescriptor = received.fileDescriptor;
    // recOk.length = received.pathLength;
    // logSendStructMessage(recOk, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    // Serialize::sendStruct(recOk, sock, clientNum);

    if(Deserialize::badLength(received.pathLength)){
        ret.errorID = static_cast<char>(Error::Type::ebadlen);
        ret.operID = static_cast<char>(ApiIDS::OPEN);
        ret.retVal = -1;
        logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
        Serialize::sendStruct(ret, sock, clientNum);
        logEndCustom("Error");
        return;
    }
    else
    {
        ret.errorID = 0;
        ret.operID = static_cast<char>(ApiIDS::OPENDIR);
        ret.retVal = 0;
        logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
        Serialize::sendStruct(ret, sock, clientNum);
        logCustom("Send struct");
    }
    Deserialize retString(received.pathLength);
    retString.receiveData(sock, clientNum);
    logCustom("The pah length is: " + std::to_string(received.pathLength));
    char* path = new char[received.pathLength];
    retString.deserializeString(path, received.pathLength);
    logReceiveStringMessage(retString.getBuffer(), "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));

    ret.retVal = api.mynfs_open(path, received.oflag, manager, gen, received.mode);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::OPEN);
    logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    Serialize::sendStruct(ret, sock, clientNum);

    delete[] path;
}

void ClientHandler::openDir(Deserialize& data, FDManager& manager, IDGen& gen)
{
    API api;
    DefRecIntData received;
    DefRetIntSendData ret;
    data.castBufferToStruct(received);
    logReceiveStructMessage(received, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));

    // DefRetIntSendData recOk;
    // recOk.operID = received.operID;
    // recOk.errorID = received.fileDescriptor;S
    // recOk.length = received.length;
    // logSendStructMessage(recOk, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    // Serialize::sendStruct(recOk, sock, clientNum);


    if(Deserialize::badLength(received.length)){
        ret.errorID = static_cast<char>(Error::Type::ebadlen);
        ret.operID = static_cast<char>(ApiIDS::OPENDIR);
        ret.retVal = -1;
        logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
        Serialize::sendStruct(ret, sock, clientNum);
        logEndCustom("Error");
        return;
    }
    else
    {
        ret.errorID = 0;
        ret.operID = static_cast<char>(ApiIDS::OPENDIR);
        ret.retVal = 0;
        logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
        Serialize::sendStruct(ret, sock, clientNum);
        logCustom("Send struct");
    }
    
    
    Deserialize retString(received.length);
    retString.receiveData(sock, clientNum);
    char* path = new char[received.length];
    retString.deserializeString(path, received.length);
    logReceiveStringMessage(retString.getBuffer(), "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));

    ret.retVal = api.mynfs_opendir(path, manager, gen);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::OPENDIR);
    logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    Serialize::sendStruct(ret, sock, clientNum);
    delete[] path;
}


void ClientHandler::readFile(Deserialize& data, FDManager& manager)
{
    API api;
    DefRecIntData rec;
    data.castBufferToStruct(rec);
    char* buf = new char[rec.length];
    logReceiveStructMessage(rec, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));

    DefRetIntSendData ret;
    ret.retVal = api.mynfs_read(rec.fileDescriptor, buf, rec.length, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::READ);
    logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    Serialize::sendStruct(ret, sock, clientNum); 

    if(ret.retVal == -1) {
        delete[] buf;
        return;
    }

    Serialize retString(ret.retVal);
    retString.serializeString(buf, ret.retVal);
    logSendStringMessage(retString.getBuffer(), "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    retString.sendData(sock, clientNum);
    delete[] buf;
}

void ClientHandler::writeFile(Deserialize& data, FDManager& manager)
{
    logStart();
    API api;
    DefRecIntData rec;
    DefRetIntSendData ret, recOk;
    data.castBufferToStruct(rec);
    logReceiveStructMessage(rec, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    
    // recOk.retVal = rec.length;
    // recOk.errorID = 0;
    // recOk.operID = rec.operID;
    // logSendStructMessage(recOk, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    // Serialize::sendStruct(recOk, sock, clientNum);

    if(Deserialize::badLength(rec.length)){
        ret.errorID = static_cast<char>(Error::Type::ebadlen);
        ret.operID = static_cast<char>(ApiIDS::WRITE);
        ret.retVal = -1;
        logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
        Serialize::sendStruct(ret, sock, clientNum);
        logEndCustom("Error");
        return;
    }
    else
    {
        ret.errorID = 0;
        ret.operID = static_cast<char>(ApiIDS::OPENDIR);
        ret.retVal = 0;
        logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
        Serialize::sendStruct(ret, sock, clientNum);
        logCustom("Send struct");
    }

    Deserialize retString(rec.length);
    retString.receiveData(sock, clientNum);

    char* toWrite = new char[rec.length];
    retString.deserializeString(toWrite, rec.length);
    logReceiveStringMessage(retString.getBuffer(), "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    ret.retVal = api.mynfs_write(rec.fileDescriptor, toWrite, rec.length, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::WRITE);
    logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    Serialize::sendStruct(ret, sock, clientNum);

    logEndCustom("Pass");
    delete[] toWrite;
}

void ClientHandler::lseekFile(Deserialize& data, FDManager& manager)
{
    API api;
    LseekRecData rec;
    DefRetIntSendData ret;
    data.castBufferToStruct(rec);
    logReceiveStructMessage(rec, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));

    ret.retVal = api.mynfs_lseek(rec.fileDescriptor, rec.whence, rec.offset, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::LSEEK);
    logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    Serialize::sendStruct(ret, sock, clientNum);
}

void ClientHandler::closeFile(Deserialize& data, FDManager& manager, bool& run)
{
    API api;
    RecDataOneLine rec;
    DefRetIntSendData ret;
    data.castBufferToStruct(rec);
    logReceiveStructMessage(rec, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    ret.retVal = api.mynfs_close(rec.fileDescriptor, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::CLOSE);
    logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    Serialize::sendStruct(ret, sock, clientNum);
    
    if(manager.empty()){
        run = false;
        logEndCustom("Zamknięto socket: " + std::to_string(sock));
    }
}

void ClientHandler::closeDir(Deserialize& data, FDManager& manager, bool& run)
{
    API api;
    RecDataOneLine rec;
    DefRetIntSendData ret;
    data.castBufferToStruct(rec);
    logReceiveStructMessage(rec, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    ret.retVal = api.mynfs_closedir(rec.fileDescriptor, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::CLOSEDIR);
    logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    Serialize::sendStruct(ret, sock, clientNum);

    if(manager.empty()){
        run = false;
        logEndCustom("Zamknięto socket: " + std::to_string(sock));
    }
}

void ClientHandler::readDir(Deserialize& data, FDManager& manager) // TODO sprawdzić czy na pewno ok
{ 
    API api;
    RecDataOneLine rec;
    DefRetIntSendData ret;
    data.castBufferToStruct(rec);
    logReceiveStructMessage(rec, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));

    char* buf = api.mynsf_readdir(rec.fileDescriptor, manager);
    ret.operID = static_cast<char>(ApiIDS::READDIR);
    ret.errorID = api.getError();

    if(buf == NULL){ // zawsze musi przy błędzie zwracać NULL
        ret.retVal = 0;        
    }
    else{
        ret.retVal = strlen(buf); // musi kończyć się nullem
    }

    logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    Serialize::sendStruct(ret, sock, clientNum);

    // TODO odebranie potwierdzenia od klienta odnośnie przesłania danych
    DefRetIntSendData recOk;
    Deserialize::receiveStruct(recOk, sock, clientNum);
    logReceiveStructMessage(recOk, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));

    if(buf != NULL)
    {
        Serialize retString(ret.retVal);
    
        if(recOk.retVal != strlen(buf))
        {
            ret.retVal = 0;
        }

        retString.serializeString(buf, ret.retVal);
        retString.sendData(sock, clientNum);
    }    

    delete[] buf;
}

void ClientHandler::fstatFile(Deserialize& data, FDManager& manager)
{
    API api;
    RecDataOneLine rec;
    fstatRetData ret;
    data.castBufferToStruct(rec);
    logReceiveStructMessage(rec, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    
    mynfs_stat my_stat = api.mynfs_fstat(rec.fileDescriptor, manager);

    ret.operID = static_cast<char>(ApiIDS::FSTAT);
    ret.errorID = api.getError();

    ret.nfs_st_valid = my_stat.nfs_st_valid;
    ret.nfs_st_size = my_stat.nfs_st_size;
    ret.nfs_st_atime = my_stat.nfs_st_atime;
    ret.nfs_st_mtime = my_stat.nfs_st_mtime;

    logSendStructMessage(ret, "\nSocket:\t" + std::to_string(sock) + "\nClientNumber:\t" + std::to_string(clientNum));
    Serialize::sendStruct(ret, sock, clientNum);
}

