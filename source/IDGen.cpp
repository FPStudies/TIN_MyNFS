/**
 * @author Mateusz Kordowski
 * 
 */

#include "IDGen.h"

IDGen::IDGen(const ID& startValue)
: values_(), nextPossibleValue_(startValue > 0 ? startValue : throw std::runtime_error("Wrong start value in IDGen")), monitor_()
{}

bool IDGen::isIdBad(ID id) const{
    if(id <= 0)
        return true;
    return false;
}

IDGen::ID IDGen::get(){
    std::lock_guard<std::mutex> guard(monitor_);
    ID ret = nextPossibleValue_;

    bool notFound = true;
    while(notFound){
        if(nextPossibleValue_ < INT32_MAX)
            ++nextPossibleValue_;
        else nextPossibleValue_ = 0;

        if(values_.find(nextPossibleValue_) == values_.end()){
            notFound = false;
        }
    }
    return ret;
}

bool IDGen::exist(const ID& id) const{
    if(isIdBad(id)) return false;

    std::lock_guard<std::mutex> guard(monitor_);
    if(values_.find(id) != values_.end())
        return true;
}

bool IDGen::dispose(const ID& id){
    if(isIdBad(id)) return false;

    std::lock_guard<std::mutex> guard(monitor_);
    values_.erase(values_.find(id));
    return true;
}