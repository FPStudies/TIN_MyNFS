/**
 * @author Mateusz Kordowski
 * 
 */

#include "FileDescriptor.h"

FileDescriptor::FileDescriptor(
    IDGen& generator,
    OffsetType initialOffset, 
    const ModeType& mode, 
    const std::string& path, 
    //std::fstream&& fileHandler
    int fileDescriptor,
    DIR* dir
)
: fdID_(generator.get()), fdOS_(fileDescriptor), mode_(mode), path_(path), generator_(generator), dir(dir)
{
    lseek(fdOS_, initialOffset, SEEK_SET);
    locConstructorCreation(this);
    /*spdlog::debug("New FileDescriptor created:\nfdID:\t" + std::to_string(fdID_) + "\nMode:\t" + static_cast<std::string>(mode_) + "\nPath:\t" + path_);*/
}

FileDescriptor::FileDescriptor(FileDescriptor&& other)
: fdID_(other.fdID_), fdOS_(other.fdOS_), mode_(std::move(other.mode_)), path_(other.path_), generator_(other.generator_), dir(other.dir)
{}
/*
FileDescriptor::FileDescriptor(const FileDescriptor& other)
:
{}
*/
FileDescriptor::operator std::string() const{
    return std::string("\nInner file descriptor id:\t" + std::to_string(fdID_) + 
        "\nOS file descriptor:\t" + std::to_string(fdOS_) +
        "\nMode:\t" + static_cast<std::string>(mode_) +
        "\nPath:\t" + path_);
}

FileDescriptor::~FileDescriptor(){
    logStart();
    generator_.dispose(fdID_); //DANGEROUS - requires the generator to still exist when invoked; remember to destroy FDManager before IDGen
    
    if(dir == NULL || dir == nullptr){
        if(fdOS_ < 0) return;
        
        if(fcntl(fdOS_, F_GETFD) < 0){
            errno = 0;
        }
        else{
            close(fdOS_); //TODO: check errors?
        }
    }
    else{
        if(closedir(dir) < 0){
            errno = 0;
        }
    }
    
    logEnd();
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

DIR* FileDescriptor::getDir() const{
    return dir;
}

void FileDescriptor::nullDir(){
    dir = NULL;
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