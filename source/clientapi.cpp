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

ClientApi::ClientApi()
{

}

int ClientApi::mynfs_open(char * host, char* path, int oflag, int mode)
{
    if(path == NULL || path == nullptr){
        std::cout << "Nie podano ścieżki" << std::endl;
        setErrno(0); // Podać prawidłowe errno
        return -1;
    }

    int pathLength = strlen(path) + 1;

    if (pathLength > 4096)
    {
        std::cout << "Za dluga sciezka" << std::endl;
        setErrno(0); // Podać prawidłowe errno
        return -1;
    }

    Client * client = nullptr;

    // Szukamy czy mamy już takie polaczenie
    for (auto const& [key, val] : clients)
    {
        if (strcmp(val->getAddress(), host) == 0)
        {
            client = clients[key];
            break;
        }
    } 

    if (client == nullptr)
    {
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
    Serialize::sendStruct(sendData, *client);
    // TODO odebrać potwierdzenie
    Serialize sendPath(pathLength);
    sendPath.serializeString(path, pathLength);
    sendPath.sendData(*client);
    
    DefRetIntSendData rec;
    Deserialize::receiveStruct(rec, *client);
    if (rec.retVal == -1)
    {
        setErrno(rec.errorID);
    }
    std::cout << "Zwrocono FD: " << rec.retVal << ", error: " << static_cast<int>(rec.errorID) << std::endl;

    //int fd = 5; // To usunac pozniej. podany do testow
    
    clients.insert(std::pair<int, Client*> (rec.retVal, client));
    return rec.retVal;
}

int ClientApi::mynfs_read(int mynfs_fd, char * buf, int len)
{
    if (!clientExist(mynfs_fd))
    {
        // nie istnieje takie polaczenie <> nie istnieje deskryptor
        // ustawic jakies errno o zlym deskryptorze
        setErrno(0); // Podać prawidłowe errno
        return -1;
    }

    Client * client = clients[mynfs_fd];

    DefRecIntData sendData;
    sendData.operID = static_cast<char>(ApiIDS::READ);
    sendData.fileDescriptor = mynfs_fd;
    sendData.length = len;
    Serialize::sendStruct(sendData, *client);

    DefRetIntSendData recData;
    Deserialize::receiveStruct(recData, *client);

    if (recData.errorID == 0)
    {
        Deserialize recString(len); // TODO sprawdzić czy dla dużych wartości działa
        recString.receiveData(*client);
        recString.deserializeString(buf, recData.retVal);

        return recData.retVal;
    }

    setErrno(recData.errorID);
    return -1;
}

int ClientApi::mynfs_write(int mynfs_fd, const char * buf, int len)
{
    if (!clientExist(mynfs_fd))
    {
        // nie istnieje takie polaczenie <> nie istnieje deskryptor
        // ustawic jakies errno o zlym deskryptorze
        setErrno(0); // Podać prawidłowe errno
        return -1;
    }

    Client * client = clients[mynfs_fd];
    DefRecIntData sendData;
    sendData.operID = static_cast<char>(ApiIDS::WRITE);
    sendData.fileDescriptor = mynfs_fd;
    sendData.length = len;
    Serialize::sendStruct(sendData, *client);// wysylamy naglowek
    
    // TODO sprawdzenie czy serwer przyjmie zgłoszenie na len bajtów
    Serialize sendStr(len);
    sendStr.serializeString(buf, len);
    sendStr.sendData(*client);// wysylamy dane do zapisu

    DefRetIntSendData recData;
    Deserialize::receiveStruct(recData, *client);
    if (recData.retVal == -1)
    {
        setErrno(recData.errorID);
    }
    std::cout << "Zwrocono ret: " << recData.retVal << ", error: " << static_cast<int>(recData.errorID) << std::endl;
    return recData.retVal;
}

int ClientApi::mynfs_lseek(int mynfs_fd, int whence, int offset)
{
    if (!clientExist(mynfs_fd))
    {
        // nie istnieje takie polaczenie <> nie istnieje deskryptor
        // ustawic jakies errno o zlym deskryptorze
        setErrno(0); // Podać prawidłowe errno
        return -1;
    }

    Client * client = clients[mynfs_fd];
    LseekRecData sendData;
    sendData.operID = static_cast<char>(ApiIDS::LSEEK);
    sendData.fileDescriptor = mynfs_fd;
    sendData.whence = whence;
    sendData.offset = offset;
    Serialize::sendStruct(sendData, *client);

    DefRetIntSendData recData;
    Deserialize::receiveStruct(recData, *client);
    if (recData.retVal == -1)
    {
        setErrno(recData.errorID);
    }
    std::cout << "Zwrocono ret: " << recData.retVal << ", error: " << static_cast<int>(recData.errorID) << std::endl;

    return recData.retVal;
}

int ClientApi::mynfs_close(int mynfs_fd)
{
    if (!clientExist(mynfs_fd))
    {
        // nie istnieje takie polaczenie <> nie istnieje deskryptor
        // ustawic jakies errno o zlym deskryptorze
        setErrno(0); // Podać prawidłowe errno
        return -1;
    }
    
    Client * client = clients[mynfs_fd];

    RecDataOneLine sendData;
    sendData.operID = static_cast<char>(ApiIDS::CLOSE);
    sendData.fileDescriptor = mynfs_fd;
    Serialize::sendStruct(sendData, *client);

    DefRetIntSendData recData;
    Deserialize::receiveStruct(recData, *client);
    if (recData.retVal == -1)
    {
        setErrno(recData.errorID);
    }
    std::cout << "Zwrocono ret: " << recData.retVal << ", error: " << static_cast<int>(recData.errorID) << std::endl;

    return recData.retVal;
}

int ClientApi::mynfs_closedir(int dirfd)
{
    if (!clientExist(dirfd))
    {
        // nie istnieje takie polaczenie <> nie istnieje deskryptor
        // ustawic jakies errno o zlym deskryptorze
        setErrno(0); // Podać prawidłowe errno
        return -1;
    }

    Client * client = clients[dirfd];
    RecDataOneLine sendData;
    sendData.operID = static_cast<char>(ApiIDS::CLOSEDIR);
    sendData.fileDescriptor = dirfd;
    Serialize::sendStruct(sendData, *client);

    DefRetIntSendData recData;
    Deserialize::receiveStruct(recData, *client);
    if (recData.retVal == -1)
    {
        setErrno(recData.errorID);
    }
    std::cout << "Zwrocono ret: " << recData.retVal << ", error: " << static_cast<int>(recData.errorID) << std::endl;

    return recData.retVal; 
}

char * ClientApi::mynfs_readdir(int dirfd)
{
    if (!clientExist(dirfd))
    {
        // nie istnieje takie polaczenie <> nie istnieje deskryptor
        // ustawic jakies errno o zlym deskryptorze
        setErrno(0); // Podać prawidłowe errno
        return NULL;
    }

    Client * client = clients[dirfd];
    RecDataOneLine sendData;
    sendData.operID = static_cast<char>(ApiIDS::READDIR);
    sendData.fileDescriptor = dirfd;
    Serialize::sendStruct(sendData, *client);

    DefRetIntSendData recData;
    Deserialize::receiveStruct(recData, *client);
    // TODO wysłanie potwierdzenia wysłania takiej ilości bajtów
    if (recData.errorID == 0)
    {
        Deserialize recStr(recData.retVal);
        recStr.receiveData(*client);
        char * dirString = new char[recData.retVal];
        recStr.deserializeString(dirString, recData.retVal);

        // dirString - zwrócona zawartość katalogu
        return dirString;
    }
    else
    {
        setErrno(recData.errorID);
        return NULL;
    }
}

int ClientApi::mynfs_opendir(char *host, char *path)
{
    return mynfs_open(host, path, O_DIRECTORY, 0);
}

mynfs_stat ClientApi::mynfs_fstat(int mynfs_fd)
{
    mynfs_stat mstat = {};
    if (!clientExist(mynfs_fd))
    {
        // nie istnieje takie polaczenie <> nie istnieje deskryptor
        // ustawic jakies errno o zlym deskryptorze
        setErrno(0); // Podać prawidłowe errno
        mstat.nfs_st_valid = false;
        std::cout<<"\nfstat nie dziala\n";
        return mstat;
    }

    Client * client = clients[mynfs_fd];
    RecDataOneLine sendData;
    sendData.operID = static_cast<char>(ApiIDS::FSTAT);
    sendData.fileDescriptor = mynfs_fd;
    Serialize::sendStruct(sendData, *client);

    fstatRetData recData;
    Deserialize::receiveStruct(recData, *client);
    if(recData.nfs_st_valid == false)
    {
        setErrno(recData.errorID);
    }

    mstat.nfs_st_valid = recData.nfs_st_valid;
    mstat.nfs_st_size = recData.nfs_st_size;
    mstat.nfs_st_atime = recData.nfs_st_atime;
    mstat.nfs_st_mtime = recData.nfs_st_mtime;

    return mstat;
}

void ClientApi::setErrno(int errorID)
{
    // TODO
}

bool ClientApi::clientExist(int fd)
{
    return !(clients.find(fd) == clients.end());
}