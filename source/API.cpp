
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

    auto& fdObject = manager.get(fd);
    if(!fdObject.isFile()){
        error_ = Error::Type::enotfile;
        logEndCustom(error_);
        return -1;
    }

    auto tmp = fdObject.getfd();
    int ret = 0;
    switch(type){
        case Seek::Set:
            ret = lseek(tmp, offset, SEEK_SET);
            break;
        case Seek::Cur:
            ret = lseek(tmp, offset, SEEK_CUR);
            break;
        case Seek::End:
            ret = lseek(tmp, offset, SEEK_END);
            break;
        default:
            error_ = Error::Type::eserv;
            logEndCustom(error_);
            return -1;
    }

    if(ret < 0){
        error_ = Error::Type::eoverflow;
        logEndCustom(error_);
        return -1;
    }

    logEndCustom("Pass");
    return ret;

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
    if(!checkPathLength(path)){
        error_ = Error::Type::enametoolong;
            logEndCustom(error_);
            return -1;
    }

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

// Te funkcje działają dobrze na fileDesrciptor systemowym z funkcji open zwykłej bo sprawdzałem różne możliwości
// Wiec wystarczy, że podepniesz pod ten swój FD co tam pisałeś


int API::mynfs_opendir(char* path, FDManager& manager, IDGen& gen, int mode)
{
    logStart();
	// Tu po prostu wystartczy wywołać tego zwykłego opena i otworzyć katalog
	// i w sumie open po stronie serwera nie potrzebuje już hosta (to tylko potrzebne u klienta), ale nie wiem jak to Mikołaj tam zakoduej póki co
	// więc zostawiłem zgodnie z dokumentacją - tu miki i mowie, ze dla serwera troche inaczej niz w dokumentacji te parametry wygladaja, ale poprawilem
	
    logEndCustom("Going to other function.");
	return mynfs_open(path, O_RDONLY, manager, gen, mode);
	// Proponuje jednak otworzyć tutaj od razu DIR *  przez funkcje fdopendir i zapisać 
	
	// ~mikolaj - tutaj inaczej zrozumialem parametry naszej metody - flags jako tryb otwarcia a mode jako uprawnienia ( tak jak to jest w linuxowym open, bo mielismy sie wzorowac ), 
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
		return NULL;
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

int API::mynfs_open(char* path, int oflag, FDManager& manager, IDGen& gen, int mode)
{
	logStart();
	
   	Mode::Operation op;
   	Mode::Type tp;
   	
   	// check the open-mode of the given file
	if((oflag & O_RDWR) == O_RDWR) op = Mode::Operation::ReadWrite;
	else if((oflag & O_RDONLY) == O_RDONLY) op = Mode::Operation::Read;
	else if((oflag & O_WRONLY) == O_WRONLY) op = Mode::Operation::Write;
	else
	{
		error_ = Error::Type::einval;
		logEndCustom(error_);
		return -1;
	}
	
	// set open flags correctly according to the oflag parameter (in case the flag is not valid)
	
	int flags;
	switch(op)
	{
		case Mode::Operation::ReadWrite:
			flags = O_RDWR;
			break;
		case Mode::Operation::Read:
			flags = O_RDONLY;
			break;
		case Mode::Operation::Write:
			flags = O_WRONLY;
			break;
		default:
			error_ = Error::Type::eserv;
			logEndCustom(error_);
			return -1;		
	}
   	if(oflag & O_CREAT) flags = flags | O_CREAT;
   	
   	// open/create a server-side file descriptor
	int fd;
	if((fd = open(path, oflag, mode)) == -1)
	{
		error_ = Error::Type::eserv;
		logEndCustom(error_);
		return -1;
	}
   	
   	// gather info about the file associated with the descriptor
	struct stat st={};
   	stat(path, &st);
   	
	// check if regular file and handle exceptions
   	if(S_ISREG(st.st_mode)) tp = Mode::Type::File;
   	else if(S_ISDIR(st.st_mode))
   	{
   		error_ = Error::Type::eisdir;
   		logEndCustom(error_);
   		close(fd);
   		return -1;
   	}
   	else
   	{
   		error_ = Error::Type::eserv;
   		logEndCustom(error_);
   		close(fd);
   		return -1;
   	}
	
	Mode md(op, tp);
	FileDescriptor fileDes(gen, 0, md, path, fd);
	logEndCustom("Pass");
	return fileDes.getID();
}

struct mynfs_stat API::mynfs_fstat(int mynfs_fd, FDManager& manager)
{

	struct mynfs_stat fileStat = {};
 
	logStart();
    if(!manager.exist(mynfs_fd)) 
    {
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
        fileStat.nfs_st_valid = false;
        return fileStat;
    }
    
    FileDescriptor fileDes( manager.get(mynfs_fd) ); 
    
  	struct stat st={};
	if(fstat(fileDes.getfd(), &st) == -1)
	{
		error_ = Error::Type::eserv;
		logEndCustom(error_);
		fileStat.nfs_st_valid = false;
		return fileStat;
	}

	fileStat.nfs_st_valid = true;
	fileStat.nfs_st_size = (int32_t)st.st_size; 
	fileStat.nfs_st_atime = (int32_t)st.st_atime; 
	fileStat.nfs_st_mtime = (int32_t)st.st_mtime;
	
	logEndCustom("Pass");
	return fileStat;
} 

int API::mynfs_read(int mynfs_fd, char* buf, int len, FDManager& manager)
{
    logStart();

    //TODO: decide what to check on client's side
    
    //check for errors
    if(len <= 0 )
    {
        logEndCustom("Commanded not to read anything.");
        return 0;
    }
    if (len > Error::MY_RW_SIZE_MAX)
    {
        error_ = Error::Type::erwsize;
        logEndCustom(error_);
        return -1;
    }
    if(!manager.exist(mynfs_fd)) {
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
        return -1;
    }

    FileDescriptor& obj_fd = manager.get(mynfs_fd);

    if (!obj_fd.isFile())
    {
        error_ = Error::Type::eisdir;
        logEndCustom(error_);
        return -1;
    }
    if (!obj_fd.isReadable())
    {
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
        return -1;
    }

    //map local mynfs_fd to file
    int sys_fd = obj_fd.getfd();

    int bytes_read = 0;

    //read data to buffer
    if ((bytes_read = read(sys_fd, buf, len)) == -1) 
    {
        error_ = Error::Type::eserv;
        logEndCustom(error_);
        return -1;
    }

    logEndCustom("File read.");
    return bytes_read;
}

int API::mynfs_write(int mynfs_fd, char* buf, int len, FDManager& manager)
{
    logStart();

    //TODO: decide what to check on client's side

    //check for errors
    if(len <= 0 )
    {
        logEndCustom("Commanded not to write anything.");
        return 0;
    }
    if (len > Error::MY_RW_SIZE_MAX)
    {
        error_ = Error::Type::erwsize;
        logEndCustom(error_);
        return -1;
    }
    if(!manager.exist(mynfs_fd)) {
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
        return -1;
    }

    FileDescriptor& obj_fd = manager.get(mynfs_fd);

    if (!obj_fd.isFile())
    {
        error_ = Error::Type::eisdir;
        logEndCustom(error_);
        return -1;
    }
    if (!obj_fd.isWriteable())
    {
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
        return -1;
    }

    //map local mynfs_fd to file
    int sys_fd = obj_fd.getfd();

    int bytes_written = 0;

    //read data to buffer
    if ((bytes_written = write(sys_fd, buf, len)) == -1) 
    {
        error_ = Error::Type::eserv;
        logEndCustom(error_);
        return -1;
    }
    //written less than expected, retrying
    if (bytes_written < len)
    {
        int new_len = len-bytes_written;
        char* new_buf = new char [new_len];
        memcpy(new_buf, buf + bytes_written, new_len);
        int new_bytes_written = 0;

        //failure sets it's own error_ value and logs proper error
        if ((new_bytes_written = mynfs_write(sys_fd, new_buf, new_len, manager)) == -1) 
        {
        delete[] new_buf;
        return -1;
        }

        delete[] new_buf;
        bytes_written += new_bytes_written;
    }
    

    logEndCustom("File written into.");
    return bytes_written;
}

bool API::checkPathLength(char* path){
    if(strlen(path) < MAX_PATH_LEN) return true;
    return false;
}


Error API::getError(){
    return error_;
}