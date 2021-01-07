#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_write test.") {
    API api;
    FDManager manager;
    IDGen generator;
    char buf[21] = "TesT";

    SECTION("Parameter error")
    {
        int ret = api.mynfs_write(0, buf, -1, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == 0);

        ret = api.mynfs_write(0, buf, Error::MY_RW_SIZE_MAX+1, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::erwsize);
    }

    SECTION("Writing nonexistent file")
    {
        int ret = api.mynfs_write(999, buf, 20, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Writing directory")
    {
        int fd = api.mynfs_opendir("/tests", manager, generator);
        int ret = api.mynfs_write(fd, buf, 20, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::eisdir);
    }

    SECTION("Writing read-only file")
    {
        int fd = api.mynfs_open("/tests/test_write_r.txt", O_RDONLY | O_CREAT, manager, generator);
        int ret = api.mynfs_write(fd, buf, 20, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Writing file")
    {
        int fd = api.mynfs_open("/tests/test_write_w.txt", O_RDWR | O_CREAT, manager, generator);
        int ret = api.mynfs_write(fd, buf, 4, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == 4);
    }

}