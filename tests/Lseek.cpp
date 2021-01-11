#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_lseek test.") {
    API api;
    IDGen generator;
    FDManager manager;

    SECTION("Parameter error")
    {
        int fd = api.mynfs_open("/tests/test_lseek.txt", O_RDWR | O_CREAT, manager, generator);
        int ret = api.mynfs_lseek(fd, -1, 0, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::einval);
    }

    SECTION("Seeking in nonexistent file")
    {
        int ret = api.mynfs_lseek(999, 2, 0, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Seeking in directory")
    {
        int fd = api.mynfs_opendir("/tests", manager, generator);
        int ret = api.mynfs_lseek(fd, 2, 0, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::enotfile);
    }

    SECTION("Seeking in file")
    {
        int fd = api.mynfs_open("/tests/test_lseek.txt", O_RDWR | O_CREAT, manager, generator);
        int ret = api.mynfs_lseek(fd, 1, 10, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret != -1);
    }
}