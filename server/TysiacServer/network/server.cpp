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


/**
 * @brief starts the server on given port
 * 
 * @param port port of the server
 */
void Server::run(size_t port)
{
	boost::system::error_code error_code;
	boost::asio::ip::tcp::endpoint endpoint(
		boost::asio::ip::tcp::v4(), static_cast<unsigned short>(port)
	);
	acceptor_.open(endpoint.protocol(), error_code);
	acceptor_.set_option(boost::asio::socket_base::reuse_address(true), error_code);
	acceptor_.bind(endpoint, error_code);
	acceptor_.listen(boost::asio::socket_base::max_connections, error_code);

	if (error_code) {
		return;
	}

	acceptor_.async_accept(socket_, endpoint_, std::bind(&Server::acceptHandler, this, beast::asio::placeholders::error));
}

/**
 * @brief stops the server and its thread
 */
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

/**
 * @brief checks whether server is accepting new connections
 */
bool Server::isAccepting() const
{
	return acceptor_.is_open();
}

/**
 * @brief handles new connections and starts sessions
 */
void Server::acceptHandler(const boost::system::error_code& error_code)
{
	if (error_code) {
		return;
	}

	std::shared_ptr<Session> session = std::make_shared<Session>(manager_, std::move(socket_));
	session->run();
	acceptor_.async_accept(socket_, endpoint_, std::bind(&Server::acceptHandler, this, beast::asio::placeholders::error));
}