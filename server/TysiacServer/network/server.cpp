#include <beast/websocket/stream.hpp>
#include <beast/core/placeholders.hpp>

#include "server.h"

Server::Server()
	: socket_(ios_)
	, acceptor_(ios_)
	, work_(ios_)
{
	thread_ = std::thread([&] { ios_.run(); });
}

Server::~Server()
{
	boost::system::error_code error_code;
	work_ = boost::none;
	ios_.dispatch([&] { acceptor_.close(error_code); });
	thread_.join();
}

void Server::run(const std::string& address, int port)
{
	boost::system::error_code error_code;

	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(address), port);

	//TODO: handle errors
	acceptor_.open(endpoint.protocol(), error_code);
	acceptor_.set_option(boost::asio::socket_base::reuse_address(true), error_code);
	acceptor_.bind(endpoint, error_code);
	acceptor_.listen(boost::asio::socket_base::max_connections, error_code);
	acceptor_.async_accept(socket_, endpoint_, std::bind(&Server::acceptHandler, this, beast::asio::placeholders::error));
}

void Server::acceptHandler(const boost::system::error_code& error_code)
{
	//TODO: handle errors
	std::shared_ptr<Session> session = std::make_shared<Session>(manager_, std::move(socket_));
	session->run();
	acceptor_.async_accept(socket_, endpoint_, std::bind(&Server::acceptHandler, this, beast::asio::placeholders::error));
}