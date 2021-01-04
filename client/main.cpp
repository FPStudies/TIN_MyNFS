#include <iostream>
#include "clientapi.hpp"

int main()
{
	ClientApi api;
	int fd = api.mynfs_open((char*)"127.0.0.1", (char*)"Dupa", 0, 0);
	api.mynfs_lseek(fd, 100, 255);
	api.mynfs_close(5);
	char * dupa = "QWEQWEWQE";
	std::cout << api.mynfs_write(5, dupa, 11) << std::endl;
	return 0;
}
