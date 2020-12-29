#include <iostream>
#include "spdlog/spdlog.h"
#include "FDManager.h"

int main(){

    spdlog::info("Welcome to spdlog!");
    FDManager manager;
    IDGen generator;
    std::fstream file("../tests/main.cpp");
    FileDescriptor fd(generator, 32, 0, Mode(Mode::Operation::Read, Mode::Type::File), "None", std::move(file));
    manager.add(std::move(fd));

    std::cout << "Hello world" << std::endl;

    return 0;
}