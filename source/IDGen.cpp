/**
 * @author Mateusz Kordowski
 * 
 */

#include "IDGen.h"

IDGen::IDGen(const ID& startValue)
: values_(), nextPossibleValue_(startValue > 0 ? startValue : throw std::runtime_error("Wrong start value in IDGen"))
{}

bool IDGen::isIdBad(ID id) const{
    if(id <= 0)
        return true;
    return false;
}

IDGen::ID IDGen::get(){
    logStart();
    ID ret = nextPossibleValue_;
    values_.emplace(ret);

    bool notFound = true;
    while(notFound){
        if(nextPossibleValue_ < INT32_MAX)
            ++nextPossibleValue_;
        else nextPossibleValue_ = 0;

        if(values_.find(nextPossibleValue_) == values_.end()){
            notFound = false;
        }
    }
    logEndCustom("Get id: " + std::to_string(ret));
    return ret;
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