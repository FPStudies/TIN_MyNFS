#ifndef API_H
#define API_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <stdint.h>

#include "FDManager.h"
#include "Errors.h"

struct mynfs_stat
{
	bool		nfs_st_valid;		/* poprawność pobrania atrybutów */
	int32_t		nfs_st_size;	    /* wielkość w bajtach (32 bity z 64) */
	int32_t    	nfs_st_atime;	    /* czas ostatniego dostępu (32 bity) */
	int32_t    	nfs_st_mtime;		/* czas ostatniej modyfikacji (32 bity) */
};


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
	int mynfs_open(char* path, int oflag, int mode, int pid, FDManager& manager, IDGen& gen);
	mynfs_stat mynfs_fstat(int mynfs_fd, int pid, FDManager& manager);


private:
    Error error_;
};

#endif
