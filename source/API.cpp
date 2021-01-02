
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

int API::mynfs_lseek(int fd, int whence, int offset, FDManager& manager, int pid){
    logStart();
    if(!manager.exist(fd, pid)) {
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

    auto& fdObject = manager.get(fd, pid);
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

int API::mynfs_close(int fd, FDManager& manager, int pid){
    logStart();
    if(!manager.exist(fd, pid)) {
        error_ = Error::Type::ebadf;
        logEndCustom(error_);
        return -1;
    }

    auto& fdObject = manager.get(fd, pid);
    auto fdOS = fdObject.getfd();
    if(close(fdOS)) {
        error_ = Error::Type::eserv;
        logEndCustom(error_);
        return -1;
    }

    if(!manager.remove(fd, pid)) {
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

int API::mynfs_open(char* path, int oflag, int mode, int pid, FDManager& manager, IDGen& gen)
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
	FileDescriptor fileDes(gen, pid, 0, md, path, fd);
	logEndCustom("Pass");
	return fileDes.getID();
}

struct mynfs_stat API::mynfs_fstat(int mynfs_fd, int pid FDManager& manager)
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
    
    FileDescriptor fileDes( manager.get(mynfs_fd, pid) ); 
    
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
	
	return fileStat;
} 











