/**
 * @author Mateusz Kordowski
 * 
 */

#include "IDGen.h"
#include <iostream>

IDGen::IDGen(const ID& startValue, const ID& maxValue)
: values_(), 
nextPossibleValue_(startValue >= 0 && startValue < INT32_MAX - 1 ? startValue : throw std::runtime_error("Wrong start value in IDGen")), 
startVal_(startValue), 
endVal_(maxValue > 0 && maxValue < INT32_MAX ? maxValue : throw std::runtime_error("Wrong max value in IDGen"))
{}

bool IDGen::isIdBad(ID id) const{
    if(id < startVal_ || id > endVal_)
        return true;
    return false;
}


IDGen::ID IDGen::get(){
    logStart();
    while(true){
        if(nextPossibleValue_ != INT32_MIN && values_.find(nextPossibleValue_) == values_.end()){
            values_.emplace(nextPossibleValue_);
            break;
        }

        if(values_.size() == (endVal_ - startVal_)){
            nextPossibleValue_ = INT32_MIN;
            break;
        }

        if(nextPossibleValue_ < endVal_ && nextPossibleValue_ >= startVal_)
            ++nextPossibleValue_;
        else nextPossibleValue_ = startVal_;
    }
    logEndCustom("Get id: " + std::to_string(nextPossibleValue_));
    return nextPossibleValue_;
}

bool IDGen::exist(const ID& id) const{
    logStart();
    if(isIdBad(id)) return false;

    if(values_.find(id) != values_.end()){
        logEndCustom("Exist id: " + std::to_string(id));
        return true;
    }
    logEndCustom("Does not exist id: "  + std::to_string(id));
    return false;
}

bool IDGen::dispose(const ID& id){
    logStart();
    if(isIdBad(id)) return false;

    auto it = values_.find(id);
    if(it == values_.end()){
        logEndCustom("Does not exist id: " + std::to_string(id));
        return false;
    }
    values_.erase(it);
    logEndCustom("Was erased id: " + std::to_string(id));
    return true;
}

size_t IDGen::size() const{
    return values_.size();
}


IDGenMonitor::IDGenMonitor(const ID& startValue, const ID& maxValue)
: IDGen(startValue, maxValue)
{}

IDGenMonitor::ID IDGenMonitor::get(){
    const std::lock_guard<std::mutex> lock(monitor_);
    nextPossibleValue_ = startVal_;
    return IDGen::get();
}

bool IDGenMonitor::exist(const ID& id) const{
    const std::lock_guard<std::mutex> lock(monitor_);
    return IDGen::exist(id);
}

bool IDGenMonitor::dispose(const ID& id){
    const std::lock_guard<std::mutex> lock(monitor_);
    return IDGen::dispose(id);

}

size_t IDGenMonitor::size() const{
    const std::lock_guard<std::mutex> lock(monitor_);
    return IDGen::size();
}