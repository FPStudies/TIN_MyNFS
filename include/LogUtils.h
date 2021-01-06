#ifndef LOG_UTILS_H
#define LOG_UTILS_H

/**
 * @author Mateusz Kordowski
 * 
 */

//#define ENABLE_LOGS

#include <string>
#ifdef ENABLE_LOGS
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"
#endif

#include "ThreadID.h"

#ifdef ENABLE_LOGS
#define logStart() \
    do {\
        /*spdlog::get(logName(threadID))->debug(std::string(__PRETTY_FUNCTION__) + "\tStarted operation.");*/ \
        spdlog::get(logName(ThreadID::getInstance().get()))->debug(std::string(__PRETTY_FUNCTION__) + "\tStarted operation."); \
    }while(false)
#else
#define logStart() ;
#endif

#ifdef ENABLE_LOGS
#define logCustom(comment) \
    do {\
        /*spdlog::get(logName(threadID))->debug(std::string(__PRETTY_FUNCTION__) + "\tMessage: " + std::string(comment));*/ \
        spdlog::get(logName(ThreadID::getInstance().get()))->debug(std::string(__PRETTY_FUNCTION__) + "\tMessage: " + std::string(comment)); \
    }while(false)
#else
#define logCustom(comment) ;
#endif

#ifdef ENABLE_LOGS
#define logEnd() \
    do {\
        spdlog::get(logName(ThreadID::getInstance().get()))->debug(std::string(__PRETTY_FUNCTION__) + "\tEnded operation."); \
    }while(false)
#else
#define logEnd() ;
#endif

#ifdef ENABLE_LOGS
#define logExitFail() \
    do {\
        spdlog::get(logName(ThreadID::getInstance().get()))->debug(std::string(__PRETTY_FUNCTION__) + "\tCould not end operation."); \
    }while(false)
#else
#define logExitFail() ;
#endif

#ifdef ENABLE_LOGS
#define logError(comment) \
     do {\
        spdlog::get(logName(ThreadID::getInstance().get()))->debug(std::string(__PRETTY_FUNCTION__) + "\tError: " + std::string(comment)); \
    }while(false)
#else
#define logError(comment) ;
#endif

#ifdef ENABLE_LOGS
#define logEndCustom(comment) \
    do {\
        logCustom(comment); \
        logEnd(); \
    }while(false)
#else
#define logEndCustom(comment) ;
#endif

#ifdef ENABLE_LOGS
#define locConstructorCreation(thisPtr) \
    do {\
        spdlog::get(logName(ThreadID::getInstance().get()))->debug(std::string(__PRETTY_FUNCTION__) + "\tCreated object: " + static_cast<std::string>(*thisPtr)); \
    }while(false)
#else
#define locConstructorCreation(thisPtr) ;
#endif

#ifdef ENABLE_LOGS
#define logInfo(comment) \
    do {\
        spdlog::get(logName(ThreadID::getInstance().get()))->info(std::string(__PRETTY_FUNCTION__) + "Message: " + std::string(comment)); \
    }while(false)
#else
#define logInfo(comment) ;
#endif

#ifdef ENABLE_LOGS
#define logInfoKnownThreadName(threadName, comment) \
    do {\
        spdlog::get(threadName)->info(std::string(__PRETTY_FUNCTION__) + " Message: " + std::string(comment)); \
    }while(false)
#else
#define logInfoKnownThreadName(threadName, comment) ;
#endif

static std::string logName(int threadID){
    return "async_logger_" + std::to_string(threadID);
}



#endif