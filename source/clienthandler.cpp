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
    std::cout << "Polaczono klienta nr "<< clientNum << ". Socket " << sock << std::endl;
    while(run)
    {
        Deserialize data(16);
        readFlag = data.receiveData(sock, clientNum);
        if (readFlag <= 0)
            run = false;
        else
        {
            id = static_cast<ApiIDS>(data.deserializeChar());
            std::cout << (int)id << std::endl;
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
                    closeFile(data, fdManager);
                    break;
                case ApiIDS::CLOSEDIR:
                    closeDir(data, fdManager);
                    break;
                case ApiIDS::READDIR:
                    readDir(data, fdManager);
                    break;
                case ApiIDS::FSTAT:
                    fstatFile(data, fdManager);
                    break;        
                default:
                    std::cout << "nieznana operacja\n"; // TODO         
            }
        }
    }
}

void ClientHandler::openFile(Deserialize& data, FDManager& manager, IDGen& gen)
{
    API api;
    OpenFileRecData received;
    data.castBufferToStruct(received);
 
    Deserialize retString(received.pathLength);
    retString.receiveData(sock, clientNum);
    char* path = new char[received.pathLength];
    retString.deserializeString(path, received.pathLength);

    DefRetIntSendData ret;
    ret.retVal = api.mynfs_open(path, received.oflag, manager, gen, received.mode);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::OPEN);
    Serialize::sendStruct(ret, sock, clientNum);

    delete path;
}

void ClientHandler::readFile(Deserialize& data, FDManager& manager)
{
    API api;
    DefRecIntData rec;
    data.castBufferToStruct(rec);
    char* buf = new char[rec.length];

    DefRetIntSendData ret;
    ret.retVal = api.mynfs_read(rec.fileDescriptor, buf, rec.length, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::READ);
    Serialize::sendStruct(ret, sock, clientNum); 

    Serialize retString(ret.retVal);
    retString.serializeString(buf, ret.retVal);
    retString.sendData(sock, clientNum);
    delete[] buf;
}

void ClientHandler::writeFile(Deserialize& data, FDManager& manager)
{
    API api;
    DefRecIntData rec;
    DefRetIntSendData ret, recOk;
    data.castBufferToStruct(rec);
    
    recOk.retVal = rec.length;
    recOk.errorID = 0;
    recOk.operID = rec.operID;
    Serialize::sendStruct(recOk, sock, clientNum);

    Deserialize retString(rec.length);
    retString.receiveData(sock, clientNum);

    char* toWrite = new char[rec.length];
    retString.deserializeString(toWrite, rec.length);
    ret.retVal = api.mynfs_write(rec.fileDescriptor, toWrite, rec.length, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::WRITE);
    Serialize::sendStruct(ret, sock, clientNum);

    delete[] toWrite;
}

void ClientHandler::lseekFile(Deserialize& data, FDManager& manager)
{
    API api;
    LseekRecData rec;
    DefRetIntSendData ret;
    data.castBufferToStruct(rec);
    ret.retVal = api.mynfs_lseek(rec.fileDescriptor, rec.whence, rec.offset, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::LSEEK);
    Serialize::sendStruct(ret, sock, clientNum);
}

void ClientHandler::closeFile(Deserialize& data, FDManager& manager)
{
    API api;
    RecDataOneLine rec;
    DefRetIntSendData ret;
    data.castBufferToStruct(rec);
    ret.retVal = api.mynfs_close(rec.fileDescriptor, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::CLOSE);
    Serialize::sendStruct(ret, sock, clientNum);
}

void ClientHandler::closeDir(Deserialize& data, FDManager& manager)
{
    API api;
    RecDataOneLine rec;
    DefRetIntSendData ret;
    data.castBufferToStruct(rec);
    ret.retVal = api.mynfs_closedir(rec.fileDescriptor, manager);
    ret.errorID = api.getError();
    ret.operID = static_cast<char>(ApiIDS::CLOSEDIR);
    Serialize::sendStruct(ret, sock, clientNum);
}

void ClientHandler::readDir(Deserialize& data, FDManager& manager) // TODO sprawdzić czy na pewno ok
{ 
    API api;
    RecDataOneLine rec;
    DefRetIntSendData ret;
    data.castBufferToStruct(rec);

    char* buf = api.mynsf_readdir(rec.fileDescriptor, manager);
    ret.operID = static_cast<char>(ApiIDS::READDIR);
    ret.errorID = api.getError();

    if(buf == NULL){ // zawsze musi przy błędzie zwracać NULL
        ret.retVal = 0;        
    }
    else{
        ret.retVal = strlen(buf); // musi kończyć się nullem
    }

    Serialize::sendStruct(ret, sock, clientNum);

    // TODO odebranie potwierdzenia od klienta odnośnie przesłania danych
    DefRetIntSendData recOk;
    Deserialize::receiveStruct(recOk, sock, clientNum);

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
    
    mynfs_stat my_stat = api.mynfs_fstat(rec.fileDescriptor, manager);

    ret.operID = static_cast<char>(ApiIDS::FSTAT);
    ret.errorID = api.getError();

    ret.nfs_st_valid = my_stat.nfs_st_valid;
    ret.nfs_st_size = my_stat.nfs_st_size;
    ret.nfs_st_atime = my_stat.nfs_st_atime;
    ret.nfs_st_mtime = my_stat.nfs_st_mtime;

    Serialize::sendStruct(ret, sock, clientNum);
}

