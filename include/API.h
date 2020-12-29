#ifndef API_H
#define API_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include "FDManager.h"
#include "Errors.h"

struct API{

    enum class Seek{
        ERROR = 0,
        Set = 1,
        Cur = 2,
        End = 3
    };
    static Seek toSeek(int error);

    int mynfs_lseek(int fd, int whence, int offset, FDManager& manager, int pid);
    int mynfs_close(int fd, FDManager& manager, int pid);
    int mynfs_unlink(char* path, FDManager& manager);

private:
    Error error_;
};

#endif