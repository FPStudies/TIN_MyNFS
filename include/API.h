
#ifndef API_H
#define API_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <stdint.h>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include "FDManager.h"
#include "Errors.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

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

    int mynfs_opendir(char* path, FDManager& manager, IDGen& gen, int mode = 0660);
    char* mynsf_readdir(int dirfd, FDManager& manager);
    int mynfs_closedir(int dirfd, FDManager& manager);

	int mynfs_open(char* path, int oflag,  FDManager& manager, IDGen& gen, int mode = 0660);
	mynfs_stat mynfs_fstat(int mynfs_fd, FDManager& manager);

    /**
     * @brief Reads specified number of bytes from file to buffer
     * 
     * @param mynfs_fd descriptor of the file from which to read
     * @param buf buffer to save read data
     * @param len maximum number of bytes to read
     * @param manager FileDescriptor manager
     * @return int on success - number of bytes read / on failure - value -1
     */
    int mynfs_read(int mynfs_fd, char* buf, int len, FDManager& manager);

    /**
     * @brief Writes specified number of bytes from buffer to file
     * 
     * @param mynfs_fd descriptor of the file to write into
     * @param buf buffer with the data to write (length of at least the value of len)
     * @param len maximum number of bytes to write
     * @param manager FileDescriptor manager
     * @return int on success - number of bytes written / on failure - value -1
     */
    int mynfs_write(int mynfs_fd, char* buf, int len, FDManager& manager);

    Error getError();

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
