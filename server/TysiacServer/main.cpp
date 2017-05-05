#include <iostream>
#include <string>

#include "network/server.h"


int main()
{
	Server server;
	server.run("192.168.43.124", 2137);
	std::cout << "RUNNING...." << std::endl;
	std::cin.get();
}