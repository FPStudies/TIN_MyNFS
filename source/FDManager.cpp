/**
 * @author Mateusz Kordowski
 * 
 */

#include "FDManager.h"

FDManager::Container::Container(FileDescriptor&& fileDescriptor)
: id(fileDescriptor.getIDConst()), path(fileDescriptor.getPath()), fd(std::move(fileDescriptor))
{}
/*
const int& FDManager::IdExtractor::operator()(const std::unique_ptr<Container>& x) const{
    return x->fd.getIDConst();
}

const std::string& FDManager::PathExtractor::operator()(const std::unique_ptr<Container>& x) const{
    return x->fd.getPathConst();
}*/


/*
bool FDManager::add(std::unique_ptr<FileDescriptor>&& fileDescriptor){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexById>();
    Container data;
    data.id = fileDescriptor->getID();
    data.path = fileDescriptor->getPath();
    auto tmp = index.insert(std::move(data));
    //auto tmp = index.emplace(fileDescriptor->getID(), std::move(fileDescriptor));
    return tmp.second;
}*/

bool FDManager::add(FileDescriptor&& fileDescriptor){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexById>();
    if(index.find(fileDescriptor.getID()) == index.end()) return false;

    auto data = std::make_unique<Container>(std::move(fileDescriptor));
    auto tmp = index.emplace(std::move(data));
    return tmp.second;
}

bool FDManager::remove(int fd, int pid){
    /*std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<int>();
    auto found = index.find(fd);
    if(found == index.end() || (*found).second->getPID() != pid) return false;
    index.erase(found);*/
    return true;
}

bool FDManager::exist(int fd){
    /*std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<int>();
    auto tmp = index.find(fd);
    if(tmp != index.end()) return true;*/
    return false;
}

bool FDManager::exist(int fd, int pid){
    /*std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<int>();
    auto tmp = index.find(fd);
    if(tmp != index.end() && (*tmp).second->getPID() == pid) return true;*/
    return false;
}

FileDescriptor& FDManager::get(int fd, int pid){
    /*std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<int>();
    auto& tmp = *index.at(fd);
    if(tmp.getPID() != pid) throw std::out_of_range("PID does not match the descryptor.");
    return tmp;*/

    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexById>();
    auto& tmp = *index.find(fd);
    if(tmp->fd.getPID() != pid) throw std::out_of_range("PID does not match the descryptor.");
    return tmp->fd;

}