#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <sys/types.h>
#include <unistd.h>
#include "Mode.h"
#include "IDGen.h"
#include "spdlog/spdlog.h"
#include "LogUtils.h"

/**
 * @brief Proxy do systemowego deskryptora plików.
 * 
 */
class FileDescriptor{
    using PIDType = int;
    using OffsetType = int;
    using ModeType = Mode;

    int fdID_;
    PIDType pid_;
    int fdOS_;
    //OffsetType offset_;
    ModeType mode_;
    //std::fstream fileHandler_; // posiada offset
    std::string path_;
    IDGen& generator_;

public:

    FileDescriptor(
        IDGen& generator, 
        const PIDType& pid, 
        const OffsetType& initialOffset, 
        const ModeType& mode, 
        const std::string& path, 
        //std::fstream&& fileHandler
        int fileDescriptor
    );
	


    ~FileDescriptor();
    FileDescriptor(FileDescriptor&& other) = default;
	FileDescriptor(const FileDescriptor& other) = default;
    int getID() const;
    const int& getIDConst() const;
    PIDType getPID() const;
    OffsetType getOffset();
    ModeType getMode() const;
    //std::fstream& getFileHandler();
    std::string getPath() const;
    const std::string& getPathConst() const;
    void setfd(int fd);
    int getfd() const;
    bool isFile() const;

    operator std::string() const;


};


#endif
