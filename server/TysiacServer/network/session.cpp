#include <beast/websocket/stream.hpp>
#include <beast/core/placeholders.hpp>
#include <beast/core.hpp>
#include <json.hpp>

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
	websocket_.async_accept(std::bind(
		&Session::acceptHandler, shared_from_this(), beast::asio::placeholders::error
	));
}

void Session::acceptHandler(const boost::system::error_code& error_code)
{
	if (error_code) {
		return;
	}

	manager_.registerSession(shared_from_this());
	welcome();
	websocket_.async_read(opcode_, buffer_, std::bind(
		&Session::readHandler, shared_from_this(), beast::asio::placeholders::error
	));
}

void Session::readHandler(const boost::system::error_code& error_code)
{
	// close connection
	if (error_code) {
		disconnect();
		return;
	}

	websocket_.async_read(opcode_, buffer_, std::bind(
		&Session::readHandler, shared_from_this(), beast::asio::placeholders::error
	));

	auto message = beast::to_string(buffer_.data());
	const std::string msg(message);
	buffer_.consume(buffer_.size());
	manager_.interpret(id_, message);
}

void Session::write(const std::string& message)
{
	queue_.push(message);
	if (!busy_) {
		busy_ = true;
		websocket_.async_write(boost::asio::buffer(queue_.front()), std::bind(
			&Session::writeHandler, shared_from_this(), beast::asio::placeholders::error
		));
	}
}

void Session::writeHandler(const boost::system::error_code& error_code)
{
	if (error_code) {
		disconnect();
		return;
	}

	queue_.pop();
	if (!queue_.empty()) {
		websocket_.async_write(boost::asio::buffer(queue_.front()), std::bind(
			&Session::writeHandler, shared_from_this(), beast::asio::placeholders::error
		));
	} else {
		busy_ = false;
	}
}

size_t Session::getId() const
{
	return id_;
}

void Session::welcome()
{
	nlohmann::json msg;
	msg["action"] = "welcome";
	msg["data"] = id_;
	write(msg.dump());
}

void Session::disconnect() const
{
	manager_.unregisterSession(id_);
	nlohmann::json msg;
	msg["action"] = "disconnect";
	manager_.interpret(id_, msg.dump());
}