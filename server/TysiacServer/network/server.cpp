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
	stop();
}

void Server::run(const std::string& address, size_t port)
{
	boost::system::error_code error_code;
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(address), port);
	acceptor_.open(endpoint.protocol(), error_code);
	acceptor_.set_option(boost::asio::socket_base::reuse_address(true), error_code);
	acceptor_.bind(endpoint, error_code);
	acceptor_.listen(boost::asio::socket_base::max_connections, error_code);

	if (error_code) {
		return;
	}

	acceptor_.async_accept(socket_, endpoint_, std::bind(&Server::acceptHandler, this, beast::asio::placeholders::error));
}

void Server::stop()
{
	boost::system::error_code error_code;
	if (isAccepting()) {
		ios_.dispatch([&] { acceptor_.close(error_code); });
	}
	work_ = boost::none;
	if (thread_.joinable()) {
		thread_.join();
	}
}

bool Server::isAccepting() const
{
	return acceptor_.is_open();
}

void Server::acceptHandler(const boost::system::error_code& error_code)
{
	if (error_code) {
		return;
	}

	std::shared_ptr<Session> session = std::make_shared<Session>(manager_, std::move(socket_));
	session->run();
	acceptor_.async_accept(socket_, endpoint_, std::bind(&Server::acceptHandler, this, beast::asio::placeholders::error));
}