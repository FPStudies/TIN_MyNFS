#include <iostream>
#include <fcntl.h>

#ifdef ENABLE_LOGS
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#endif
#include "FDManager.h"
#include "API.h"

int main(){

    #ifdef ENABLE_LOGS
    spdlog::info("Welcome to spdlog!");
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    spdlog::set_level(spdlog::level::debug);
    #endif

    FDManager manager;
    IDGen generator;
    int fdOS = open("tests/main.cpp", O_RDONLY);
    if(fdOS == -1) throw std::runtime_error("Could not open file");
    FileDescriptor fd(generator, getpid(), 0, Mode(Mode::Operation::Read, Mode::Type::File), "None", fdOS);
    manager.add(std::move(fd));

    API api;
    api.mynfs_lseek(fd.getID(), 1, 6, manager);
    
    auto stat = api.mynfs_fstat(fd.getID(), manager);
    
    std::cout<<"size of the file: " << stat.nfs_st_size << std::endl;

    close(fdOS);
    
    int nfs_fd = api.mynfs_open("tests/test.txt", O_RDONLY | O_CREAT, manager, generator); 
    
    std::cout << "Server-side fd: " << nfs_fd << std::endl;

    std::cout << "Hello world" << std::endl;

    return 0;
}
