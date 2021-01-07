#include "catch2/catch_amalgamated.hpp"
#include "API.h"

TEST_CASE ("MyNFS_open test.") 
{
    API api;
    FDManager manager;
    IDGen generator;

    //never happens; O_RDONLY == 0 so it's the "default"
    /*
    SECTION("Parameter errors") 
    {
        int fd = api.mynfs_open("test1.txt", 8, manager, generator);
        REQUIRE(fd == -1);
        REQUIRE(api.getError().get() == Error::Type::einval);
    }
    */

    SECTION("Opening nonexistent file")
    {
        int fd = api.mynfs_open("/tests/test_nonexistent.txt", O_RDWR, manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd == -1);
        REQUIRE(api.getError().get() == Error::Type::eserv);
    }

    //creates a file with no extension. how to differentiate?
    /*
    SECTION("Creating nonexistent directory")
    {
        int fd = api.mynfs_open("/tests/nonexistent", O_RDONLY | O_CREAT | O_DIRECTORY, manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd == -1);
        REQUIRE(api.getError().get() == Error::Type::eserv);
    }
    */

    SECTION("Creating new read-write file")
    {
        int fd = api.mynfs_open("/tests/test_open_creat_rw.txt", O_RDWR | O_CREAT, manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        //close(manager.get(fd).getfd());
        //unlink("test_open_creat_rw.txt");
        REQUIRE(fd != -1);
        REQUIRE(manager.exist(fd));
        REQUIRE(manager.get(fd).isFile());
        REQUIRE(manager.get(fd).getMode().operation == Mode::Operation::ReadWrite);
    }

    SECTION("Opening read-write file")
    {
        int fd = api.mynfs_open("/tests/test_open_creat_rw.txt", O_RDWR, manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd != -1);
        REQUIRE(manager.exist(fd));
        REQUIRE(manager.get(fd).isFile());
        REQUIRE(manager.get(fd).getMode().operation == Mode::Operation::ReadWrite);
    }

    SECTION("Creating new write only file")
    {
        int fd = api.mynfs_open("/tests/test_open_creat_w.txt", O_WRONLY | O_CREAT, manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd != -1);
        REQUIRE(manager.exist(fd));
        REQUIRE(manager.get(fd).isFile());
        REQUIRE(manager.get(fd).getMode().operation == Mode::Operation::Write);
    }

    SECTION("Opening write only file")
    {
        int fd = api.mynfs_open("/tests/test_open_creat_w.txt", O_WRONLY, manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd != -1);
        REQUIRE(manager.exist(fd));
        REQUIRE(manager.get(fd).isFile());
        REQUIRE(manager.get(fd).getMode().operation == Mode::Operation::Write);
    }

    SECTION("Creating new read only file")
    {
        int fd = api.mynfs_open("/tests/test_open_creat_r.txt", O_RDONLY | O_CREAT, manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd != -1);
        REQUIRE(manager.exist(fd));
        REQUIRE(manager.get(fd).isFile());
        REQUIRE(manager.get(fd).getMode().operation == Mode::Operation::Read);
    }

    SECTION("Opening read only file")
    {
        int fd = api.mynfs_open("/tests/test_open_creat_r.txt", O_RDONLY, manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd != -1);
        REQUIRE(manager.exist(fd));
        REQUIRE(manager.get(fd).isFile());
        REQUIRE(manager.get(fd).getMode().operation == Mode::Operation::Read);
    }

    SECTION("Opening directory")
    {
        int fd = api.mynfs_open("/tests", O_RDONLY | O_DIRECTORY, manager, generator);
        //std::cout<< Error::toString(api.getError().get()) << std::endl;
        REQUIRE(fd != -1);
        REQUIRE(manager.exist(fd));
        REQUIRE(manager.get(fd).isDirectory());
    }

}