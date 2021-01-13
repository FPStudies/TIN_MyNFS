#include <iostream>

#ifdef ENABLE_LOGS
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#endif

#include "clientapi.h"

int main(){

    #ifdef ENABLE_LOGS
    spdlog::info("Welcome to spdlog!");
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    spdlog::set_level(spdlog::level::debug);
    #endif

    // for(int i = 0; i < 10; ++i) {
        ClientApi api;
        auto fd = api.mynfs_open("78.88.237.18", "/file.txt", O_RDWR | O_CREAT, 0660); // hardcoded for now
        char* mes = "vjik";
        std::cout<<"\nafter nfs open\n";

        api.mynfs_lseek(fd, SEEK_END, 0);

        api.mynfs_write(fd, mes, strlen(mes)+1);
        std::cout<<"\nafter nfs write\n";

        auto res = api.mynfs_fstat(fd);
        std::cout<<"\nafter nfs fstat\n";

        std::cout<<res.nfs_st_size<<std::endl;
        // api.mynfs_lseek(fd, SEEK_END, 0);
        std::cout<<"\nafter nfs fstat\n";

        api.mynfs_write(fd, mes, 7);
        std::cout<<"\nafter nfs write\n";

        res = api.mynfs_fstat(fd);
        std::cout<<"\nafter nfs fstat\n";

        std::cout<<res.nfs_st_size<<std::endl;

        char* rs = new char[1000]();

        api.mynfs_lseek(fd, SEEK_SET, 0);
        api.mynfs_read(fd, rs, 7);
        std::cout<<rs<<std::endl;
        api.mynfs_close(fd);
        std::cout<<"\nafter nfs close\n";
        auto dfd = api.mynfs_opendir("78.88.237.18", "/source");
        rs = api.mynfs_readdir(dfd);
        if(rs != NULL) if(strlen(rs) > 0) std::cout<<rs<<std::endl;
        api.mynfs_closedir(dfd);
        delete[] rs;

   // }


    
    return 0;
}
