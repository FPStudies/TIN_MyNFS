#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_close test.") {
    API api;
    FDManager manager;
    IDGen generator;

    SECTION("Closing nonexistent file")
    {
        int ret = api.mynfs_close(999, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Closing open file")
    {
        int fd = api.mynfs_open("/tests/test_close.txt", O_RDWR | O_CREAT, manager, generator);
        int ret = api.mynfs_close(fd, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret != -1);
        REQUIRE(!manager.exist(fd));
    }
}