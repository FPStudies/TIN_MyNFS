#include <iostream>
#include "clientapi.hpp"

int main()
{
	ClientApi api;
	//api.mynfs_closedir(123);
	int fd = api.mynfs_open((char*)"127.0.0.1", (char*)"Dupa", 0, 0);
	std::cout << "Otrzymano fd: " << fd << std::endl;
	char* ret = api.mynfs_readdir(5);
	if(ret == NULL){
		std::cout << "Zwrócono NULL" << std::endl;
	}
	else{
		std::cout << std::string(ret) << std::endl;
	}

	if(api.mynfs_closedir(5) == -1){
		std::cout << "Błąd zamykania" << std::endl;
	}
	
	return 0;
}
