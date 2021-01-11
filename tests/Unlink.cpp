#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_unlink test.") {
    API api;
    IDGen generator;
    FDManager manager;

    SECTION("Unlinking nonexistent file")
    {
        int ret = api.mynfs_unlink("/tests/test_nonexistent.txt", manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::enoent);
    }

    SECTION("Unlinking directory")
    {
        int fd = api.mynfs_opendir("/tests", manager, generator);
        int ret = api.mynfs_unlink("/tests", manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret == -1);
        REQUIRE(api.getError().get() == Error::Type::eisdir);
    }

    SECTION("Unlinking file")
    {
        int fd = api.mynfs_open("/tests/test_unlink.txt", O_RDWR | O_CREAT, manager, generator);
        int ret = api.mynfs_unlink("/tests/test_unlink.txt", manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(ret != -1);
        REQUIRE(!manager.exist(fd));
        REQUIRE(open("/tests/test_unlink.txt", O_RDWR) == -1);
    }
}