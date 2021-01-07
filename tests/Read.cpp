#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_read test.") {
    API api;
    FDManager manager;
    IDGen generator;
    char buf[21] = "";

    SECTION("Parameter error")
    {
        int ret = api.mynfs_read(0, buf, -1, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == 0);

        ret = api.mynfs_read(0, buf, Error::MY_RW_SIZE_MAX+1, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::erwsize);
    }

    SECTION("Reading nonexistent file")
    {
        int ret = api.mynfs_read(999, buf, 20, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Reading directory")
    {
        int fd = api.mynfs_opendir("/tests", manager, generator);
        int ret = api.mynfs_read(fd, buf, 20, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::eisdir);
    }

    SECTION("Reading write-only file")
    {
        int fd = api.mynfs_open("/tests/test_read_w.txt", O_WRONLY | O_CREAT, manager, generator);
        int ret = api.mynfs_read(fd, buf, 20, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Reading empty file")
    {
        int fd = api.mynfs_open("/tests/test_read_r.txt", O_RDWR | O_CREAT, manager, generator);
        int ret = api.mynfs_read(fd, buf, 20, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == 0);
    }

    SECTION("Reading file")
    {
        int fd = api.mynfs_open("/tests/test_read_r.txt", O_RDWR | O_CREAT, manager, generator);
        api.mynfs_write(fd, "TesT", 4, manager);
        api.mynfs_lseek(fd, 0, 0, manager);
        int ret = api.mynfs_read(fd, buf, 20, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == 4);
    }
}