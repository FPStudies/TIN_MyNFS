/**
 * @author Mateusz Kordowski
 * 
 */

#include "FileDescriptor.h"

FileDescriptor::FileDescriptor(
    IDGen& generator, 
    const PIDType& pid, 
    const OffsetType& initialOffset, 
    const ModeType& mode, 
    const std::string& path, 
    std::fstream&& fileHandler
)
: fdID_(generator.get()), pid_(pid), mode_(mode), fileHandler_(std::move(fileHandler)), path_(path), generator_(generator)
{
    fileHandler_.seekg(initialOffset);
    spdlog::debug("New FileDescriptor created:\nfdID:\t" + std::to_string(fdID_) + "\nPID:\t" + 
        std::to_string(pid_) + "\nMode:\t" + static_cast<std::string>(mode_) + "\nPath:\t" + path_);
}

FileDescriptor::~FileDescriptor(){
    generator_.dispose(fdID_);
}

int FileDescriptor::getID() const{
    return fdID_;
}

const int& FileDescriptor::getIDConst() const{
    return fdID_;
}

FileDescriptor::PIDType FileDescriptor::getPID() const{
    return pid_;
}

FileDescriptor::OffsetType FileDescriptor::getOffset(){
    return fileHandler_.tellg();
}

FileDescriptor::ModeType FileDescriptor::getMode() const{
    return mode_;
}

std::fstream& FileDescriptor::getFileHandler(){
    return fileHandler_;
}

std::string FileDescriptor::getPath() const{
    return path_;
}

const std::string& FileDescriptor::getPathConst() const{
    return path_;
}