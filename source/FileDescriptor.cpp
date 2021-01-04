/**
 * @author Mateusz Kordowski
 * 
 */

#include "FileDescriptor.h"

FileDescriptor::FileDescriptor(
    IDGen& generator,
    const OffsetType& initialOffset, 
    const ModeType& mode, 
    const std::string& path, 
    //std::fstream&& fileHandler
    int fileDescriptor
)
: fdID_(generator.get()), fdOS_(fileDescriptor), mode_(mode), path_(path), generator_(generator)
{
    lseek(fdOS_, initialOffset, SEEK_SET);
    locConstructorCreation(this);
    /*spdlog::debug("New FileDescriptor created:\nfdID:\t" + std::to_string(fdID_) + "\nMode:\t" + static_cast<std::string>(mode_) + "\nPath:\t" + path_);*/
}

FileDescriptor::operator std::string() const{
    return std::string("\nInner file descriptor id:\t" + std::to_string(fdID_) + 
        "\nOS file descriptor:\t" + std::to_string(fdOS_) +
        "\nMode:\t" + static_cast<std::string>(mode_) +
        "\nPath:\t" + path_);
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

FileDescriptor::ModeType FileDescriptor::getMode() const{
    return mode_;
}

std::string FileDescriptor::getPath() const{
    return path_;
}

const std::string& FileDescriptor::getPathConst() const{
    return path_;
}

void FileDescriptor::setfd(int fd){
    fdOS_ = fd;
}

int FileDescriptor::getfd() const{
    return fdOS_;
}

bool FileDescriptor::isFile() const
{
    if (mode_.type == Mode::Type::File) return true;
    return false;
}

bool FileDescriptor::isDirectory() const
{
    if (mode_.type == Mode::Type::Catalog) return true;
    return false;
}

bool FileDescriptor::isReadable() const
{
    if (isFile() && (mode_.operation == Mode::Operation::Read || mode_.operation == Mode::Operation::ReadWrite) ) return true;
    return false;
}

bool FileDescriptor::isWriteable() const
{
    if (isFile() && (mode_.operation == Mode::Operation::Write || mode_.operation == Mode::Operation::ReadWrite) ) return true;
    return false;
}