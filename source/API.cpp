
#include "API.h"


API::Seek API::toSeek(int error){
    switch(error){
        case 1:
            return Seek::Set;
        case 2:
            return Seek::Cur;
        case 3:
            return Seek::End;
        default:
            return Seek::ERROR;

    }
}

int API::mynfs_lseek(int fd, int whence, int offset, FDManager& manager){
    logStart();
    if(!manager.exist(fd)) {
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
        return -1;
    }
    auto type = toSeek(whence);
    if(type == Seek::ERROR) {
        error_ = Error::Type::einval;
        logEndCustom(error_);
        return -1;
    }

    // TODO check offset

    auto& fdObject = manager.get(fd);
    auto tmp = fdObject.getfd();
    switch(type){
        case Seek::Set:
            lseek(tmp, offset, SEEK_SET);
            break;
        case Seek::Cur:
            lseek(tmp, offset, SEEK_CUR);
            break;
        case Seek::End:
            lseek(tmp, offset, SEEK_END);
            break;
        default:
            error_ = Error::Type::eserv;
            logEndCustom(error_);
            return -1;
    }

    logEndCustom("Pass");
    return 0;

}

int API::mynfs_close(int fd, FDManager& manager){
    logStart();
    if(!manager.exist(fd)) {
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
        return -1;
    }

    auto& fdObject = manager.get(fd);
    auto fdOS = fdObject.getfd();
    if(close(fdOS)) {
        error_ = Error::Type::eserv;
        logEndCustom(error_);
        return -1;
    }

    if(!manager.remove(fd)) {
        logEndCustom("Could not remove a file. Inner error.");
        throw std::runtime_error("Could not remove a file."); // poważny błąd w kodzie
    }

    logEndCustom("Pass");
    return 0;
}

int API::mynfs_unlink(char* path, FDManager& manager){
    logStart();
    // TODO sprawdzić długość ścieżki

    struct stat sb;
    if(stat(path, &sb) == 0) {
        if(S_ISDIR(sb.st_mode)){
            error_ = Error::Type::eisdir;
            logEndCustom(error_);
            return -1;
        }
        // OK
    }
    else{
        error_ = Error::Type::enoent;
        logEndCustom(error_);
        return -1;
    }

    auto fdObjects = manager.get(path);
    int lastFD = -1;
    for(auto& it : fdObjects){
        auto& myfd = it.get();
        lastFD = myfd.getfd();
        close(lastFD);
        if(!manager.remove(lastFD)){
            logEndCustom("Could not remove a file. Inner error.");
            throw std::runtime_error("Could not remove a file."); // poważny błąd w kodzie
        }
    }
    if(unlink(path)){
        error_ = Error::Type::eio;
        logEndCustom(error_);
        return -1;
    }

    logEndCustom("Pass");
    return 0;
}


#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>

// Te funkcje działają dobrze na fileDesrciptor systemowym z funkcji open zwykłej bo sprawdzałem różne możliwości
// Wiec wystarczy, że podepniesz pod ten swój FD co tam pisałeś


int API::mynsf_opendir(char* host, char* path)
{
    logStart();
	// Tu po prostu wystartczy wywołać tego zwykłego opena i otworzyć katalog
	// i w sumie open po stronie serwera nie potrzebuje już hosta (to tylko potrzebne u klienta), ale nie wiem jak to Mikołaj tam zakoduej póki co
	// więc zostawiłem zgodnie z dokumentacją

    logEndCustom("Going to other function.");
	return mynfs_open(host, path, O_DIRECTORY, O_RDONLY);
	// Proponuje jednak otworzyć tutaj od razu DIR *  przez funkcje fdopendir i zapisać 
}

char* API::mynsf_readdir(int dirfd)
{
    logStart();
	if (dirfd < 0)
	{
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
		return NULL;
	}

	// Chyba trzeba b�dzie gdzie� sk�adowa� ten DIR, �eby go nie otwiera� ca�y czas
	// Można otworzyć w funkcji opendir i zapisać do klasy, a później z niego korzystać.
	// Zgodnie z tym co wklejałem z dokumentacji
	// After a successful call to fdopendir(), fd is used internally by the implementation, and should not otherwise be used by the application.

	DIR* dir = fdopendir(dirfd);
	if (dir == NULL)
	{
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
		return NULL;
	}

	struct dirent * de;
	std::string str;

	while ((de = readdir(dir)) != NULL)
	{
		if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
		{
			str += "'";
			str += de->d_name;
			str += "'-";
		}
	}

	if (str.empty())
	{
		// pusty katalog
        logEndCustom("Pass. Empty string.");
		return "";
	}

	str = str.substr(0, str.size() - 1);

	char* returnChar = new char[str.length() + 1];
	strcpy(returnChar, str.c_str());

	// Zgodnie z treścią zadania zwraca liste plików i katalogów. Oddzielone tak jak w dokumentacji.
    logEndCustom("Pass");
	return returnChar;
}

int API::mynfs_closedir(int dirfd)
{
    logStart();
	if (dirfd < 0)
	{
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
		return -1;
	}

	DIR* dir = fdopendir(dirfd);
	if (dir == NULL)
	{
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
		return -1;
	}

	if (closedir(dir) == 0)
	{
		// Udalo sie zamknac
        logEndCustom("Pass.");
		return 0;
	}

    error_ = Error::Type::eserv;
    logEndCustom(error_);
	return -1;
}