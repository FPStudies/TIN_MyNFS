#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch_amalgamated.hpp"
#include <vector>
#include "clientapi.h"


TEST_CASE("many connections", "[connections]")
{
    int n = 100;
   for(int i = 0; i < n; ++i)
   {
        ClientApi api;
        int fd = api.mynfs_open("127.0.0.1", "/file.txt", O_RDWR | O_CREAT, 0660); 
        REQUIRE(fd >= 0);
        api.mynfs_close(fd);
   }
    REQUIRE(true);
}

TEST_CASE("write and fstat tests", "[write_fstat]")
{
    ClientApi api;
    int fd = api.mynfs_open("127.0.0.1", "/file.txt", O_RDWR | O_CREAT, 0660); 
    auto res = api.mynfs_fstat(fd);
    int initSize = res.nfs_st_size;

    char* mes = "Siemka";
    api.mynfs_lseek(fd, SEEK_END, 0);
    api.mynfs_write(fd, mes, strlen(mes) + 1);
    res = api.mynfs_fstat(fd);
    int dif = res.nfs_st_size - initSize;

    REQUIRE(dif == 7);

    api.mynfs_close(fd);
}

TEST_CASE("Read and lseek tests", "[read_lseek]")
{
    ClientApi api;
    int fd = api.mynfs_open("127.0.0.1", "/testCase2.txt", O_RDWR | O_CREAT, 0660);

    REQUIRE(fd >= 0);

    char * returnString = new char[12];
    api.mynfs_read(fd, returnString, 9);

    char * test1 = "TestCase2";
    REQUIRE((strcmp(test1, returnString)) == 0);
    
    // Test LSeek SEEK_START
    api.mynfs_lseek(fd, SEEK_SET, 9);
    api.mynfs_read(fd, returnString, 9);

    char * test2 = "LseekTest";
    REQUIRE((strcmp(test2, returnString)) == 0);

    // Test LSeek SEEK_CUR (0)
    api.mynfs_lseek(fd, SEEK_CUR, 7);
    api.mynfs_read(fd, returnString, 12);

    char * test3 = "Seek_EndTest";
    REQUIRE((strcmp(test3, returnString)) == 0);

    int retVal = api.mynfs_close(fd);

    REQUIRE(retVal == 0);
}

TEST_CASE("ReadDir tests", "[ReadDir]")
{
    ClientApi api;
    int dirFD = api.mynfs_opendir("127.0.0.1", "/TestCase3_DIR");
    REQUIRE(dirFD >= 0);

    char * returnDir = api.mynfs_readdir(dirFD);
    REQUIRE((strcmp(returnDir, "'testcase3.txt'")) == 0);

    int retVal = api.mynfs_closedir(dirFD);
    REQUIRE(retVal == 0);
}

