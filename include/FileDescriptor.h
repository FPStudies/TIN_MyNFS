#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <fstream>
#include "Mode.h"
#include "IDGen.h"
#include "spdlog/spdlog.h"

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
    //OffsetType offset_;
    ModeType mode_;
    std::fstream fileHandler_; // posiada offset
    std::string path_;
    IDGen& generator_;

public:

    FileDescriptor(
        IDGen& generator, 
        const PIDType& pid, 
        const OffsetType& initialOffset, 
        const ModeType& mode, 
        const std::string& path, 
        std::fstream&& fileHandler
    );

    ~FileDescriptor();
    FileDescriptor(FileDescriptor&& other) = default;

    int getID() const;
    const int& getIDConst() const;
    PIDType getPID() const;
    OffsetType getOffset();
    ModeType getMode() const;
    std::fstream& getFileHandler();
    std::string getPath() const;
    const std::string& getPathConst() const;

    operator std::string() const{
        return std::string("\nFile descriptor id:\t" + std::to_string(fdID_) + 
            "\nPID:\t" + std::to_string(pid_) +
            "\nMode:\t" + static_cast<std::string>(mode_) +
            "\nPath:\t" + path_);
    }

    //bool setOffset(const OffsetType& offset);
    //void setMode(const ModeType& mode);



};


#endif