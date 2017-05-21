#pragma once

#include <thread>
#include <boost/asio.hpp>
#include <boost/optional.hpp>

#include "manager.h"

class Server {
public:
	Server();
	~Server();
	Server(const Server& server) = delete;
	Server& operator=(const Server& server) = delete;
	void run(const std::string& address, size_t port);
	size_t connected() const;

private:
	boost::asio::io_service ios_;
	SessionManager manager_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::endpoint endpoint_;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::optional<boost::asio::io_service::work> work_;
	std::thread thread_;

	void acceptHandler(const boost::system::error_code& error_code);
};