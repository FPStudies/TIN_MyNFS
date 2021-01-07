#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_readdir test.") {
    API api;
    FDManager manager;
    IDGen generator;

    SECTION("Parameter error")
    {
        char* ret = api.mynsf_readdir(-1, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == NULL);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Reading nonexistent directory")
    {
        char* ret = api.mynsf_readdir(999, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == NULL);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Reading file")
    {
        int fd = api.mynfs_open("/tests/test_readdir.txt", O_RDWR | O_CREAT, manager, generator);
        char* ret = api.mynsf_readdir(fd, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == NULL);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Empty directory")
    {
        int fd = api.mynfs_opendir("/tests/test_readdir_empty", manager, generator);
        char* ret = api.mynsf_readdir(fd, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE((strcmp(ret, "")) == 0);
    }

    SECTION("Reading directory")
    {
        int fd = api.mynfs_opendir("/tests/test_readdir", manager, generator);
        int fdf = api.mynfs_open("/tests/test_readdir/test.txt", O_RDWR | O_CREAT, manager, generator);
        char* ret = api.mynsf_readdir(fd, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(!strcmp(ret, "'test.txt'"));
    }
}