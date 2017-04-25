#include <beast/websocket/stream.hpp>
#include <beast/core/placeholders.hpp>
#include <beast/core.hpp>

#include "session.h"
#include "manager.h"

Session::Session(SessionManager& manager, boost::asio::ip::tcp::socket&& socket)
	: manager_(manager)
	, websocket_(std::move(socket))
	, id_([] {
		static size_t id = 0;
		return id++;
	}())
	, busy_(false)
{}

void Session::run()
{
	websocket_.async_accept(std::bind(&Session::acceptHandler, shared_from_this(), beast::asio::placeholders::error));
}

void Session::acceptHandler(const boost::system::error_code& error_code)
{
	//TODO: handle errors
	manager_.registerSession(shared_from_this());
	welcome();
	websocket_.async_read(opcode_, buffer_, std::bind(&Session::readHandler, shared_from_this(), beast::asio::placeholders::error));
}

void Session::readHandler(const boost::system::error_code& error_code)
{
	//TODO: handle errors
	if (error_code) {
		manager_.unregisterSession(id_);
		return;
	}
	websocket_.async_read(opcode_, buffer_, std::bind(&Session::readHandler, shared_from_this(), beast::asio::placeholders::error));
	auto message = beast::to_string(buffer_.data());
	const std::string msg(message);
	buffer_.consume(buffer_.size());
	manager_.interpret(message);
}

void Session::write(const std::string& message)
{
	queue_.push(message);
	if (!busy_) {
		busy_ = true;
		websocket_.async_write(boost::asio::buffer(queue_.front()), std::bind(&Session::writeHandler, shared_from_this(), beast::asio::placeholders::error));
	}
}

void Session::writeHandler(const boost::system::error_code& error_code)
{
	//TODO: handle errors
	queue_.pop();
	if (!queue_.empty()) {
		websocket_.async_write(boost::asio::buffer(queue_.front()), std::bind(&Session::writeHandler, shared_from_this(), beast::asio::placeholders::error));
	} else {
		busy_ = false;
	}
}

std::size_t Session::getId() const
{
	return id_;
}

void Session::welcome()
{
	write(std::to_string(id_));
}