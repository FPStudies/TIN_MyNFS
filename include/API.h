
#ifndef API_H
#define API_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <stdint.h>

#include "FDManager.h"
#include "Errors.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct mynfs_stat
{
	bool		nfs_st_valid;		/* poprawność pobrania atrybutów */
	int32_t		nfs_st_size;	    /* wielkość w bajtach (32 bity z 64) */
	int32_t    	nfs_st_atime;	    /* czas ostatniego dostępu (32 bity) */
	int32_t    	nfs_st_mtime;		/* czas ostatniej modyfikacji (32 bity) */
};


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

    int mynfs_opendir(char* host, char* path, FDManager& manager, IDGen& gen, int mode = 0660, int pid = 0);
    char* mynsf_readdir(int dirfd);
    int mynfs_closedir(int dirfd);

	int mynfs_open(char* path, int oflag,  FDManager& manager, IDGen& gen, int mode = 0660, int pid = 0);
	mynfs_stat mynfs_fstat(int mynfs_fd, FDManager& manager);


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
