#pragma once

#include <beast/websocket/stream.hpp>
#include <queue>

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
public:
	Session(SessionManager& manager, boost::asio::ip::tcp::socket&& socket);
	void run();
	void write(const std::string& message);
	size_t getId() const;

private:
	SessionManager& manager_;
	beast::websocket::stream<boost::asio::ip::tcp::socket> websocket_;
	beast::websocket::opcode opcode_;
	beast::streambuf buffer_;
	size_t id_;
	std::queue<std::string> queue_;
	bool busy_;

	void acceptHandler(const boost::system::error_code& error_code);
	void readHandler(const boost::system::error_code& error_code);
	void writeHandler(const boost::system::error_code& error_code);
	void welcome();
};