#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_fstat test.") {
    API api;
    FDManager manager;
    IDGen generator;

    SECTION("Stating nonexistent file")
    {
        auto stat = api.mynfs_fstat(999, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(stat.nfs_st_valid == false);
        REQUIRE(api.getError().get() == Error::Type::ebadf);
    }

    SECTION("Stating open file")
    {
        int fd = api.mynfs_open("/tests/test_fstat.txt", O_RDWR | O_CREAT, manager, generator);
        auto stat = api.mynfs_fstat(fd, manager);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(stat.nfs_st_valid == true);
    }
}