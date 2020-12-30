
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