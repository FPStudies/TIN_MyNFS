#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_opendir test.") {
    API api;
    IDGen generator;
    FDManager manager;

    //creates new directory instead of error
    /*
    SECTION("Opening nonexistent directory")
    {
        int fd = api.mynfs_opendir("/nonexistent ", manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd == -1);
        REQUIRE(api.getError().get() == Error::Type::eserv);
    }
    */

    SECTION("Opening directory")
    {
        int fd = api.mynfs_opendir("/tests", manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd != -1);
        REQUIRE(manager.exist(fd));
        REQUIRE(manager.get(fd).isDirectory());
    }
}