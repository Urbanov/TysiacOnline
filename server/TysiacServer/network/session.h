#pragma once

#include <beast/websocket/stream.hpp>
#include <queue>

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
private:
	SessionManager& manager_;
	beast::websocket::stream<boost::asio::ip::tcp::socket> websocket_;
	beast::websocket::opcode opcode_;
	beast::streambuf buffer_;
	std::size_t id_;
	std::queue<std::string> queue_;
	bool busy_;

public:
	Session(SessionManager& manager, boost::asio::ip::tcp::socket&& socket);
	void run();
	void acceptHandler(const boost::system::error_code& error_code);
	void readHandler(const boost::system::error_code& error_code);
	void write(const std::string& message);
	void writeHandler(const boost::system::error_code& error_code);
	void welcome();
	std::size_t getId() const;
};