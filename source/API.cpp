
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
    if(!manager.exist(fd, pid)) {
        error_ = Error::Type::ebadf;
        return -1;
    }
    auto type = toSeek(whence);
    if(type == Seek::ERROR) {
        error_ = Error::Type::einval;
        return -1;
    }

    // TODO check offset

    auto& fdObject = manager.get(fd, pid);
    auto& tmp = fdObject.getFileHandler();
    switch(type){
        case Seek::Set:
            tmp.seekg(offset, tmp.beg);
            break;
        case Seek::Cur:
            tmp.seekg(offset, tmp.cur);
            break;
        case Seek::End:
            tmp.seekg(offset, tmp.end);
            break;
    }

    return 0;

}

int API::mynfs_close(int fd, FDManager& manager, int pid){
    if(!manager.exist(fd, pid)) {
        error_ = Error::Type::ebadf;
        return -1;
    }

    auto& fdObject = manager.get(fd, pid);
    auto& file = fdObject.getFileHandler();
    file.close();
    if(file.fail()) {
        error_ = Error::Type::eserv;
        return -1;
    }

    if(!manager.remove(fd, pid)) throw std::runtime_error("Could not remove a file."); // poważny błąd kodu

    return 0;
}

int API::mynfs_unlink(char* path, FDManager& manager){
    if(!manager.exist(path)) {
        error_ = Error::Type::ebadf;
        return -1;
    }

    auto& fdObject = manager.get(path);
    auto& file = fdObject.getFileHandler();

    file.close();
    if(file.fail()) {
        error_ = Error::Type::eserv;
        return -1;
    }
    return 0;
}