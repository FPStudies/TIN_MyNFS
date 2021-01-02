#ifndef API_H
#define API_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include "FDManager.h"
#include "Errors.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct API{
    static const unsigned int MAX_PATH_LEN = 4096;

    enum class Seek{
        ERROR = 0,
        Set = 1,
        Cur = 2,
        End = 3
    };
    static Seek toSeek(int error);

    int mynfs_lseek(int fd, int whence, int offset, FDManager& manager);
    int mynfs_close(int fd, FDManager& manager);
    int mynfs_unlink(char* path, FDManager& manager);

    int mynsf_opendir(char* host, char* path);
    char* mynsf_readdir(int dirfd);
    int mynfs_closedir(int dirfd);


private:
    Error error_;

    /**
     * @brief Sprawdza czy długość ścieżki mieści się w limicie.
     * 
     * @param path 
     * @return true - Mieści się w limicie.
     * @return false - Nie mieści się w limicie.
     */
    bool checkPathLength(char* path);
};

#endif