#include <iostream>
#include "spdlog/spdlog.h"
#include "clientapi.h"

int main(){

    #ifdef ENABLE_LOGS
    spdlog::info("Welcome to spdlog!");
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    spdlog::set_level(spdlog::level::debug);
    #endif


    std::cout << "Hello world" << std::endl;

    ClientApi api;
    auto fd = api.mynfs_open("127.0.0.1", "file.txt", O_RDWR | O_CREAT, 0660); // hardcoded for now
    char* mes = "Siemka";
    std::cout<<"\nafter nfs open\n";

    api.mynfs_write(fd, mes, 7);
    std::cout<<"\nafter nfs write\n";

    auto res = api.mynfs_fstat(fd);
    std::cout<<"\nafter nfs fstat\n";

    std::cout<<res.nfs_st_size;
    api.mynfs_lseek(fd, SEEK_END, 0);
    std::cout<<"\nafter nfs fstat\n";

    api.mynfs_write(fd, mes, 7);
    std::cout<<"\nafter nfs write\n";

    res = api.mynfs_fstat(fd);
    std::cout<<"\nafter nfs fstat\n";

    std::cout<<res.nfs_st_size;

    api.mynfs_close(fd);
    std::cout<<"\nafter nfs close\n";

    auto dfd = api.mynfs_opendir("127.0.0.1", "source");
    char * buf = api.mynfs_readdir(dfd);
    std::cout<<buf;
    api.mynfs_closedir(dfd);

    return 0;
}