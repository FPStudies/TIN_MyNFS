/**
 * @file ThreadID.cpp
 * @author Mateusz Kordowski
 * @brief 
 * @version 0.1
 * @date 2021-01-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "ThreadID.h"

ThreadID::Container::Container(std::thread::id osID, int id)
: os_id_(osID), id_(id)
{}

bool ThreadID::add(int id){
    std::lock_guard<std::mutex> lock(mutex_);
    auto threadid = std::this_thread::get_id();
    auto& tmp = map_.get<IndexByID>();
    if(tmp.find(id) != tmp.end()){
        return false;
    }

    auto ptr = std::make_unique<Container>(threadid, id);

    auto ret = tmp.emplace(std::move(ptr));
    return ret.second;
}

bool ThreadID::remove(int id){
    std::lock_guard<std::mutex> lock(mutex_);
    auto& index = map_.get<IndexByID>();
    auto found = index.find(id);
    if(found == index.end()) {
        return false;
    }
    index.erase(found);
    return true;
}

int ThreadID::get(){
    std::lock_guard<std::mutex> lock(mutex_);
    auto& index = map_.get<IndexByOSID>();
    auto tmp = index.find(std::this_thread::get_id());
    if(tmp == map_.end()){
        return INT32_MIN;
    }
    return (*tmp)->id_;
}