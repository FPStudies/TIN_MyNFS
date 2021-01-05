#include <iostream>
#include <fcntl.h>

#ifdef ENABLE_LOGS
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#endif
#include "FDManager.h"
#include "API.h"
#include "server.h"

int main(){

    #ifdef ENABLE_LOGS
    spdlog::info("Welcome to spdlog!");
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    spdlog::set_level(spdlog::level::debug);
    #endif

    
    Server server;
	server.run();
    

    // FDManager manager;
    // IDGen generator;
    // int fdOS = open("tests/test.txt", O_RDWR);
    // if(fdOS == -1) throw std::runtime_error("Could not open file");
    // FileDescriptor fd(generator, 0, Mode(Mode::Operation::ReadWrite, Mode::Type::File), "None", fdOS);
    // manager.add(std::move(fd));

    // API api;
    // char buf_r[30] = "";
    // api.mynfs_lseek(fd.getID(), 1, 1, manager);
    // std::cout << api.mynfs_write(fd.getID(), "TesT", strlen("TesT"), manager) << std::endl;
    // api.mynfs_lseek(fd.getID(), 1, 0, manager);
    // std::cout << api.mynfs_read(fd.getID(), buf_r, 30, manager) << std::endl;
    // buf_r[29] = '\0';
    // std::cout << buf_r << std::endl;

    // auto stat = api.mynfs_fstat(fd.getID(), manager);
    
    // std::cout<<"\nsize of the file: " << stat.nfs_st_size << std::endl;

    // close(fdOS);
    
    // int nfs_fd = api.mynfs_open("tests/test.txt", O_RDONLY | O_CREAT, manager, generator); 
    
    // std::cout << "Server-side fd: " << nfs_fd << std::endl;

    // std::cout << "Hello world" << std::endl;


    return 0;
}
