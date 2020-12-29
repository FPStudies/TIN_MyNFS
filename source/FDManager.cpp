/**
 * @author Mateusz Kordowski
 * 
 */

#include "FDManager.h"

FDManager::Container::Container(FileDescriptor&& fileDescriptor)
: id(fileDescriptor.getIDConst()), path(fileDescriptor.getPath()), fd(std::move(fileDescriptor))
{}

bool FDManager::add(FileDescriptor&& fileDescriptor){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexById>();
    if(index.find(fileDescriptor.getID()) == index.end()) return false;

    auto data = std::make_unique<Container>(std::move(fileDescriptor));
    auto tmp = index.emplace(std::move(data));
    return tmp.second;
}

bool FDManager::remove(int fd, int pid){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexById>();
    auto found = index.find(fd);
    if(found == index.end() || (*found)->fd.getPID() != pid) return false;
    index.erase(found);
    return true;
}

bool FDManager::exist(int fd){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexById>();
    auto tmp = index.find(fd);
    if(tmp != index.end()) return true;
    return false;
}

bool FDManager::exist(int fd, int pid){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexById>();
    auto tmp = index.find(fd);
    if(tmp != index.end() && (*tmp)->fd.getPID() == pid) return true;
    return false;
}

bool FDManager::exist(const std::string& path){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexByPath>();
    auto tmp = index.find(path);
    if(tmp != index.end()) return true;
    return false;
}

bool FDManager::exist(const std::string& path, int pid){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexByPath>();
    auto tmp = index.find(path);
    if(tmp != index.end() && (*tmp)->fd.getPID() == pid) return true;
    return false;
}

FileDescriptor& FDManager::get(int fd, int pid){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexById>();
    auto& tmp = *index.find(fd);
    if(tmp->fd.getPID() != pid) throw std::out_of_range("PID does not match the descryptor.");
    return tmp->fd;
}

FileDescriptor& FDManager::get(const std::string& path){
    std::lock_guard<std::mutex> guard(monitor_);
    auto& index = fd_.get<IndexByPath>();
    auto& tmp = *index.find(path);
    return tmp->fd;
}