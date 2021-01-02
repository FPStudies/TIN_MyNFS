#include <iostream>
#include "clientapi.hpp"

int main()
{
	ClientApi api;
	//api.mynfs_closedir(123);
	int fd = api.mynfs_open((char*)"127.0.0.1", (char*)"Dupa", 0, 0);
	std::cout << "Otrzymano fd: " << fd << std::endl;
	api.mynfs_readdir(5);
	return 0;
}
