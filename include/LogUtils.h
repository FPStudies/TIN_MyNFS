#ifndef LOG_UTILS_H
#define LOG_UTILS_H

/**
 * @author Mateusz Kordowski
 * 
 */

#include <string>
#ifdef ENABLE_LOGS
#include "spdlog/spdlog.h"
#endif

#ifdef ENABLE_LOGS
#define logStart() \
    do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\tStarted operation."); \
    }while(false)
#else
#define logStart() ;
#endif

#ifdef ENABLE_LOGS
#define logCustom(comment) \
    do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\tMessage: " + std::string(comment)); \
    }while(false)
#else
#define logCustom(comment) ;
#endif

#ifdef ENABLE_LOGS
#define logEnd() \
    do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\tEnded operation."); \
    }while(false)
#else
#define logEnd() ;
#endif

#ifdef ENABLE_LOGS
#define logExitFail() \
    do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\tCould not end operation."); \
    }while(false)
#else
#define logExitFail() ;
#endif

#ifdef ENABLE_LOGS
#define logError(comment) \
     do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\tError: " + std::string(comment)); \
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
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\tCreated object: " + static_cast<std::string>(*thisPtr)); \
    }while(false)
#else
#define locConstructorCreation(thisPtr) ;
#endif

#ifdef ENABLE_LOGS
#define logSendStruct(structure) \
    do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\tSending structure: " + static_cast<std::string>(structure)); \
    }while(false)
#else
#define logSendStruct(structure) ;
#endif

#ifdef ENABLE_LOGS
#define logSendStructMessage(structure, message) \
    do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\tSending structure: " + static_cast<std::string>(structure) + \
        "\nMessage: " + std::string(message)); \
    }while(false)
#else
#define logSendStructMessage(structure, message) ;
#endif

#ifdef ENABLE_LOGS
#define logReceiveStructMessage(structure, message) \
    do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\nReceived structure: " + static_cast<std::string>(structure) + \
        "\nMessage: " + std::string(message)); \
    }while(false)
#else
#define logReceiveStructMessage(structure, message) ;
#endif

#ifdef ENABLE_LOGS
#define logSendStringMessage(str, message) \
    do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\nSending string: " + std::string(str) + \
        "\nMessage: " + std::string(message)); \
    }while(false)
#else
#define logSendStringMessage(str, message) ;
#endif

#ifdef ENABLE_LOGS
#define logReceiveStringMessage(str, message) \
    do {\
        spdlog::get("basic_logger")->debug(std::string(__PRETTY_FUNCTION__) + "\nReceived string: " + std::string(str) + \
        "\nMessage: " + std::string(message)); \
    }while(false)
#else
#define logReceiveStringMessage(str, message) ;
#endif

#endif