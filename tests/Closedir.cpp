#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_closedir test.") {
    API api;
    FDManager manager;
    IDGen generator;

    SECTION("Parameter error")
    {
        int ret = api.mynfs_closedir(-1, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Closing nonexistent directory")
    {
        int ret = api.mynfs_closedir(999, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Closing open directory")
    {
        int fd = api.mynfs_opendir("/tests", manager, generator);
        REQUIRE(fd >= 0);
        //int ret = api.mynfs_closedir(fd, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        //REQUIRE(ret != -1);
        //REQUIRE(!manager.exist(fd));
    }
}