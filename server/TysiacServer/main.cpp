#include <iostream>

#include "network/server.h"

int main()
{
	Server server;
	server.run("127.0.0.1", 2137);
	std::cout << "RUNNING..." << std::endl;

	boost::asio::io_service ios;
	boost::asio::signal_set signals(ios, SIGINT, SIGTERM);
	signals.async_wait([&](boost::system::error_code const&, int) {
		std::cout << "STOPPING..." << std::endl;
		server.stop();
	});
	ios.run();
}