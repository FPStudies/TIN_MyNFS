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

    for(int i = 0; i< 100; ++i){
    ClientApi api;
    int img_size = 9860;
    int sleep_time = 1000000;
    auto fd = api.mynfs_open("127.0.0.1", "/kwiat.jpeg", O_RDONLY, 0660); // hardcoded for now
    //api.mynfs_lseek(fd, SEEK_SET, 0);
    char* img = new char[img_size]();
    api.mynfs_read(fd, img, img_size);
    //usleep(sleep_time);
    api.mynfs_close(fd);
    //usleep(sleep_time);
    std::cout << "Read" <<std::endl;
    auto fd_2 = api.mynfs_open("127.0.0.1", "/test_img2.jpg", O_RDWR | O_CREAT, 0660); // hardcoded for now
    //api.mynfs_lseek(fd_2, SEEK_SET, 0);
    api.mynfs_write(fd_2, img, img_size);
    //usleep(sleep_time);
    api.mynfs_close(fd_2);
    //usleep(sleep_time);
    std::cout << "Written" <<std::endl;
    delete[] img;
    }
    return 0;
}
