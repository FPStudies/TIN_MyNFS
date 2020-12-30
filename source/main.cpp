#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "FDManager.h"
#include "API.h"

int main(){

    spdlog::info("Welcome to spdlog!");
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    spdlog::set_level(spdlog::level::debug);

    FDManager manager;
    IDGen generator;
    std::fstream file("../tests/main.cpp");
    FileDescriptor fd(generator, 32, 0, Mode(Mode::Operation::Read, Mode::Type::File), "None", std::move(file));
    manager.add(std::move(fd));

    API api;
    api.mynfs_lseek(fd.getID(), 1, 6, manager, 32);

    std::cout << "Hello world" << std::endl;

    return 0;
}