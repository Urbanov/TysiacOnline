#include <iostream>
#include <string>

#include "network/server.h"


int main()
{
	Server server;
	server.run("127.0.0.1", 2137);
	std::cout << "RUNNING...." << std::endl;
	std::cin.get();
}