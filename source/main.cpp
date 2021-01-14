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

    spdlog::flush_on(spdlog::level::info);  // 
    spdlog::flush_on(spdlog::level::debug);  // aby podczas ctrl ^ c zapisało logi do plików. 
    #endif


//78.88.237.18
//127.0.0.1
/*
    for(int i = 0; i < 100; ++i) {
        ClientApi api;
        auto fd = api.mynfs_open("78.88.237.18", "/file_fp.txt", O_RDWR | O_CREAT, 0660); // hardcoded for now
        char* mes = "\\TesT/";
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

        char* rs = new char[1000]();

        api.mynfs_lseek(fd, SEEK_SET, 0);
        api.mynfs_read(fd, rs, (i+1)*strlen(mes));
        //std::cout<<rs<<std::endl;
        api.mynfs_close(fd);
        std::cout<<"\nafter nfs close\n";
        //delete[] rs;
        
        rs = NULL;
        auto dfd = api.mynfs_opendir("78.88.237.18", "/source");
        rs = api.mynfs_readdir(dfd);
        if(rs != NULL) if(strlen(rs) > 0) std::cout<<rs<<std::endl;
        api.mynfs_closedir(dfd);
        
        delete[] rs;
        

    }
*/
    //int img_size = 1000;
    //int sleep_time = 1000000;

    //for (int i=0; i<5; ++i) 
    //{
        ClientApi api;
        auto fd = api.mynfs_open("78.88.237.18", "/test_img.jpg", O_RDONLY, 0660); // hardcoded for now
        auto stat = api.mynfs_fstat(fd);
        int img_size = stat.nfs_st_size;
        //api.mynfs_lseek(fd, SEEK_SET, 0);
        char* img = new char[img_size]();
        api.mynfs_read(fd, img, img_size);
        //usleep(sleep_time);
        api.mynfs_close(fd);
        //usleep(sleep_time);
        std::cout << "Read img" <<std::endl;
        auto fd_2 = api.mynfs_open("78.88.237.18", "/test_img_fp.jpg", O_RDWR | O_CREAT, 0660); // hardcoded for now
        //api.mynfs_lseek(fd_2, SEEK_SET, 0);
        api.mynfs_write(fd_2, img, img_size);
        //usleep(sleep_time);
        api.mynfs_close(fd_2);
        //usleep(sleep_time);
        std::cout << "Written img" <<std::endl;
        delete[] img;
    //}


    return 0;
}
