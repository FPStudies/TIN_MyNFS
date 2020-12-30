#ifndef LOG_UTILS_H
#define LOG_UTILS_H

/**
 * @author Mateusz Kordowski
 * 
 */

#include <string>
#include "spdlog/spdlog.h"

#define logStart() \
    do {\
        spdlog::debug(std::string(__PRETTY_FUNCTION__) + "\tStarted operation."); \
    }while(false)

#define logCustom(comment) \
    do {\
        spdlog::debug(std::string(__PRETTY_FUNCTION__) + "\tMessage: " + std::string(comment)); \
    }while(false)

#define logEnd() \
    do {\
        spdlog::debug(std::string(__PRETTY_FUNCTION__) + "\tEnded operation."); \
    }while(false)

#define logExitFail() \
    do {\
        spdlog::debug(std::string(__PRETTY_FUNCTION__) + "\tCould not end operation."); \
    }while(false)

#define logError(comment) \
     do {\
        spdlog::debug(std::string(__PRETTY_FUNCTION__) + "\tError: " + std::string(comment)); \
    }while(false)

#define logEndCustom(comment) \
    do {\
        logCustom(comment); \
        logEnd(); \
    }while(false)

#define locConstructorCreation(thisPtr) \
    do {\
        spdlog::debug(std::string(__PRETTY_FUNCTION__) + "\tCreated object: " + static_cast<std::string>(*thisPtr)); \
    }while(false)

#endif