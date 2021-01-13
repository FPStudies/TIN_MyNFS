#include <iostream>

#ifdef ENABLE_LOGS
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#endif

#include "clientapi.h"
#include <fstream>

int main(){

    #ifdef ENABLE_LOGS
    spdlog::info("Welcome to spdlog!");
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    spdlog::set_level(spdlog::level::debug);
    #endif
    for(int i = 0; i < 100; ++i)
    {
        ClientApi api;
        auto fd = api.mynfs_open("78.88.237.18", "/file.txt", O_RDWR | O_CREAT, 0660); // hardcoded for now
        char* mes = "vjik";
        std::cout<<"\nafter nfs open\n";

        api.mynfs_lseek(fd, SEEK_END, 0);

        api.mynfs_write(fd, mes, strlen(mes));
        std::cout<<"\nafter nfs write\n";

        auto res = api.mynfs_fstat(fd);
        std::cout<<"\nafter nfs fstat\n";

        std::cout<<res.nfs_st_size<<std::endl;
        // api.mynfs_lseek(fd, SEEK_END, 0);
        std::cout<<"\nafter nfs fstat\n";

        api.mynfs_write(fd, mes, strlen(mes));
        std::cout<<"\nafter nfs write\n";

        res = api.mynfs_fstat(fd);
        std::cout<<"\nafter nfs fstat\n";

        std::cout<<res.nfs_st_size<<std::endl;

        char* rs = new char[2000]();

        api.mynfs_lseek(fd, SEEK_SET, 0);

        api.mynfs_read(fd, rs, 10);
        std::cout<<rs<<std::endl;

        api.mynfs_close(fd);
        usleep(10000);


        fd = api.mynfs_open("78.88.237.18", "/test_img.jpg", O_RDWR | O_CREAT, S_IRWXU |  S_IRWXG | S_IRWXO); // hardcoded for now
        res = api.mynfs_fstat(fd);
        std::cout<<"\nafter nfs fstat\n";

        std::cout<<res.nfs_st_size<<std::endl;

        api.mynfs_lseek(fd, SEEK_SET, 0);
        api.mynfs_read(fd, rs, 500);
        std::cout<<rs<<std::endl;
        api.mynfs_close(fd);
        std::cout<<"\nafter nfs close\n";
        usleep(100000);
        auto dfd = api.mynfs_opendir("78.88.237.18", "/");
        rs = api.mynfs_readdir(dfd);
        if(rs != NULL) if(strlen(rs) > 0) std::cout<<rs<<std::endl;
        api.mynfs_closedir(dfd);
        delete[] rs;
    }
    return 0;
}
