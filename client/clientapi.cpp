#include "clientapi.hpp"

ClientApi::ClientApi()
{

}

int ClientApi::mynfs_open(char * host, char* path, int oflag, int mode)
{
    int pathLength = strlen(path);

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

    char clientSendMSG[16];
    clientSendMSG[0] = (int)ApiIDS::OPEN;
    clientSendMSG[1] = 0; // padding
    clientSendMSG[2] = 0;
    clientSendMSG[3] = 0;
    datagrams.serializeInt(&clientSendMSG[4], oflag, 4);
    datagrams.serializeInt(&clientSendMSG[8], mode, 4);
    datagrams.serializeInt(&clientSendMSG[12], pathLength, 4);

    client->sendProtocol(clientSendMSG, 16); // wysylamy naglowek
    client->sendProtocol(path, pathLength + 1); // wysylamy sciezke


    char returnBuffer[8];
    int readFlag = client->readProtocol(returnBuffer, sizeof(returnBuffer));

    int fd = datagrams.deserializeInt(&returnBuffer[4], 4);
    int errorID = 0;
    if (fd == -1)
    {
        errorID = datagrams.deserializeInt(&returnBuffer[1], 1);
        setErrno(errorID);
    }
    std::cout << "Zwrocono FD: " << fd << ", error: " << errorID << std::endl;

    fd = 5; // To usunac pozniej. podany do testow
    
    clients.insert(std::pair<int, Client*> (fd, client));
    return fd;
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

    char clientSendMSG[8];
    clientSendMSG[0] = (int)ApiIDS::READ;
    clientSendMSG[1] = 0; // padding
    datagrams.serializeInt(&clientSendMSG[2], mynfs_fd, 2);
    datagrams.serializeInt(&clientSendMSG[4], len, 4);

    client->sendProtocol(clientSendMSG, sizeof(clientSendMSG));

    
    char returnBuffer[4104];
    int readFlag = client->readProtocol(returnBuffer, sizeof(returnBuffer));

    int errorID = datagrams.deserializeInt(&returnBuffer[1], 1);

    if (errorID == -1)
    {
        // errorID = -1 to oznacza brak bledu i przyjmujemy string
        // Trzeba to uwzględnić w dokumentacji

        int stringLength = datagrams.deserializeInt(&returnBuffer[4], 4);
        strcpy(buf, &returnBuffer[8]);

        return stringLength;
    }
    else
    {
        setErrno(errorID);
        return -1;
    }
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
    char clientSendMSG[16];
    clientSendMSG[0] = (int)ApiIDS::WRITE;
    clientSendMSG[1] = 0; // padding
    datagrams.serializeInt(&clientSendMSG[2], mynfs_fd, 2);
    datagrams.serializeInt(&clientSendMSG[4], len, 4);

    client->sendProtocol(clientSendMSG, sizeof(clientSendMSG)); // wysylamy naglowek
    client->sendProtocol(const_cast<char*>(buf), len + 1); // wysylamy dane do zapisu

    char returnBuffer[8];
    int readFlag = client->readProtocol(returnBuffer, sizeof(returnBuffer));

    int retVal = datagrams.deserializeInt(&returnBuffer[4], 4);
    int errorID = 0;
    if (retVal == -1)
    {
        errorID = datagrams.deserializeInt(&returnBuffer[1], 1);
        setErrno(errorID);
    }
    std::cout << "Zwrocono ret: " << retVal << ", error: " << errorID << std::endl;
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

    char clientSendMSG[12];
    clientSendMSG[0] = (int)ApiIDS::LSEEK;
    clientSendMSG[1] = 0; // padding
    datagrams.serializeInt(&clientSendMSG[2], mynfs_fd, 2);
    datagrams.serializeInt(&clientSendMSG[4], whence, 4);
    datagrams.serializeInt(&clientSendMSG[8], offset, 4);

    client->sendProtocol(clientSendMSG, sizeof(clientSendMSG));

    char returnBuffer[8];
    int readFlag = client->readProtocol(returnBuffer, sizeof(returnBuffer));


    int retVal = datagrams.deserializeInt(&returnBuffer[4], 4);
    int errorID = 0;
    if (retVal == -1)
    {
        errorID = datagrams.deserializeInt(&returnBuffer[1], 1);
        setErrno(errorID);
    }
    std::cout << "Zwrocono ret: " << retVal << ", error: " << errorID << std::endl;

    return retVal;
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

    char clientSendMSG[12];
    clientSendMSG[0] = (int)ApiIDS::CLOSE;
    clientSendMSG[1] = 0; // padding
    datagrams.serializeInt(&clientSendMSG[2], mynfs_fd, 2);
    client->sendProtocol(clientSendMSG, sizeof(clientSendMSG));


    char returnBuffer[8];
    int readFlag = client->readProtocol(returnBuffer, sizeof(returnBuffer));

    int retVal = datagrams.deserializeInt(&returnBuffer[4], 4);
    int errorID = 0;
    if (retVal == -1)
    {
        errorID = datagrams.deserializeInt(&returnBuffer[1], 1);
        setErrno(errorID);
    }
    std::cout << "Zwrocono ret: " << retVal << ", error: " << errorID << std::endl;

    return retVal;

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
    char clientSendMSG[4];

    clientSendMSG[0] = (int)ApiIDS::CLOSEDIR;
    clientSendMSG[1] = 0; // padding
    datagrams.serializeInt(&clientSendMSG[2], dirfd, 2);

    client->sendProtocol(&clientSendMSG[0], sizeof(clientSendMSG));

    char returnBuffer[8];
    int readFlag = client->readProtocol(returnBuffer, sizeof(returnBuffer));

    int retVal = datagrams.deserializeInt(&returnBuffer[4], 4);
    int errorID = 0;
    if (retVal == -1)
    {
        errorID = datagrams.deserializeInt(&returnBuffer[1], 1);
        setErrno(errorID);
    }
    std::cout << "Zwrocono ret: " << retVal << ", error: " << errorID << std::endl;

    return retVal; 
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
    char clientSendMSG[4];
    clientSendMSG[0] = (int)ApiIDS::READDIR;
    clientSendMSG[1] = 0; // padding
    datagrams.serializeInt(&clientSendMSG[2], dirfd, 2);

    client->sendProtocol(&clientSendMSG[0], sizeof(clientSendMSG));

    char returnBuffer[4104];
    int readFlag = client->readProtocol(returnBuffer, sizeof(returnBuffer));
    
    int errorID = datagrams.deserializeInt(&returnBuffer[1], 1);

    if (errorID == -1)
    {
        // errorID = -1 to oznacza brak bledu i przyjmujemy string
        int stringLength = datagrams.deserializeInt(&returnBuffer[4], 4);

        char * dirString = new char[stringLength];
        strcpy(dirString, &returnBuffer[8]);

        // dirString - zwrócona zawartość katalogu
        return dirString;
    }
    else
    {
        setErrno(errorID);
        return NULL;
    }
}

int ClientApi::mynfs_opendir(char *host, char *path)
{
    return mynfs_open(host, path, O_DIRECTORY, 0);
}

void ClientApi::setErrno(int errorID)
{
    // TODO
}

bool ClientApi::clientExist(int fd)
{
    return !(clients.find(fd) == clients.end());
}