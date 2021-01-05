#include <iostream>
#include "spdlog/spdlog.h"
#include "clientapi.h"

int main(){

    spdlog::info("Welcome to spdlog!");

    std::cout << "Hello world" << std::endl;

    ClientApi api;
    auto fd = api.mynfs_open("192.168.0.23", "file.txt", O_RDWR | O_CREAT, 0660); // hardcoded for now
    char* mes = "Siemka";
    api.mynfs_write(fd, mes, 7);
    auto res = api.mynfs_fstat(fd);
    std::cout<<res.nfs_st_size;
    api.mynfs_lseek(fd, SEEK_END, 0);
    api.mynfs_write(fd, mes, 7);
    res = api.mynfs_fstat(fd);
    std::cout<<res.nfs_st_size;

    api.mynfs_close(fd);


    return 0;
}