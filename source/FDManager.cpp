/**
 * @author Mateusz Kordowski
 * 
 */

#include "FDManager.h"

FDManager::Container::Container(FileDescriptor&& fileDescriptor)
: id(fileDescriptor.getIDConst()), path(fileDescriptor.getPath()), fd(std::move(fileDescriptor))
{}

bool FDManager::add(FileDescriptor& fileDescriptor){
    logStart();
    auto& index = fd_.get<IndexById>();
    if(index.find(fileDescriptor.getID()) != index.end()) {
        logEndCustom("Already exist");
        return false;
    }

    auto data = std::make_unique<Container>(std::move(fileDescriptor));
    auto tmp = index.emplace(std::move(data));
    logEndCustom("Added");
    return tmp.second;
}

bool FDManager::remove(int fd){
    logStart();
    auto& index = fd_.get<IndexById>();
    auto found = index.find(fd);
    if(found == index.end()) {
        logEndCustom("Does not exist");
        return false;
    }
    index.erase(found);
    logEndCustom("Removed");
    return true;
}

bool FDManager::exist(int fd){
    logStart();
    auto& index = fd_.get<IndexById>();
    auto tmp = index.find(fd);
    if(tmp != index.end()) {
        logEndCustom("Does exist");
        return true;
    }
    logEndCustom("Does not exist");
    return false;
}

bool FDManager::exist(const std::string& path){
    logStart();
    auto& index = fd_.get<IndexByPath>();
    auto tmp = index.equal_range(path);
    if(tmp.first != tmp.second) {
        logEndCustom("Does exist");
        return true;
    }
    logEndCustom("Does not exist");
    return false;
}

FileDescriptor& FDManager::get(int fd){
    logStart();
    auto& index = fd_.get<IndexById>();
    auto& tmp = *index.find(fd);
    logEndCustom(static_cast<std::string>(tmp->fd));
    return tmp->fd;
}

FDManager::FDVector FDManager::get(const std::string& path){
    logStart();
    FDVector ret;
    auto& index = fd_.get<IndexByPath>();
    auto tmp = index.equal_range(path);

    for(auto& it : boost::make_iterator_range(tmp)){
        ret.push_back(boost::reference_wrapper<FileDescriptor>(it->fd));
        logCustom(static_cast<std::string>(it->fd));
    }

    logEndCustom("Returned vector.");
    return ret;
}

bool FDManager::empty() const{
    return fd_.empty();
}