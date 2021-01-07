#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "Mode.h"
#include "IDGen.h"
#include "LogUtils.h"
#include "dirent.h"

/**
 * @brief Proxy do systemowego deskryptora plików.
 * 
 */
class FileDescriptor{
    using OffsetType = int;
    using ModeType = Mode;

    int fdID_;
    int fdOS_;
    //OffsetType offset_;
    ModeType mode_;
    //std::fstream fileHandler_; // posiada offset
    std::string path_;
    IDGen& generator_;
    DIR* dir;

public:

    FileDescriptor(
        IDGen& generator, 
        OffsetType initialOffset, 
        const ModeType& mode, 
        const std::string& path, 
        //std::fstream&& fileHandler
        int fileDescriptor,
        DIR* dir = nullptr
    );
	


    ~FileDescriptor();
    FileDescriptor(FileDescriptor&& other);
	FileDescriptor(const FileDescriptor& other) = delete;

    int getID() const;
    const int& getIDConst() const;
    ModeType getMode() const;
    std::string getPath() const;
    const std::string& getPathConst() const;
    void setfd(int fd);
    int getfd() const;
    DIR* getDir() const;
    void nullDir();

    operator std::string() const;

    /**
     * @brief Checks if descriptor points to an open file
     * 
     * @return true descriptor points to an open file
     * @return false descriptor doesn't point to an open file
     */
    bool isFile() const;

    /**
     * @brief Checks if descriptor points to a directory
     * 
     * @return true descriptor points to a directory
     * @return false descriptor doesn't point to a directory
     */
    bool isDirectory() const;

    /**
     * @brief Checks if descriptor points to a file opened to read
     * 
     * @return true descriptor points to a file opened to read
     * @return false descriptor doesn't point to a file opened to read
     */
    bool isReadable() const;

    /**
     * @brief Checks if descriptor points to a file opened to write
     * 
     * @return true descriptor points to a file opened to write
     * @return false descriptor doesn't point to a file opened to write
     */
    bool isWriteable() const;

};


#endif
