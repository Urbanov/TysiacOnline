#include <boost/test/unit_test.hpp>
#include <beast/core.hpp>
#include <json.hpp>

#include "../network/server.h"

struct Client {
	boost::asio::io_service ios;
	boost::asio::ip::tcp::resolver resolver;
	boost::asio::ip::tcp::socket socket;
	beast::websocket::stream<boost::asio::ip::tcp::socket&> ws;
	beast::streambuf buffer;
	beast::websocket::opcode op;

	Client()
		: resolver(ios)
		, socket(ios)
		, ws(socket)
	{
		boost::asio::connect(socket, resolver.resolve(boost::asio::ip::tcp::resolver::query{ "127.0.0.1", "2137" }));
		ws.handshake("127.0.0.1", "/");
	}

	void write(const std::string& msg)
	{
		ws.write(boost::asio::buffer(msg));
	}

	const std::string read()
	{
		ws.read(op, buffer);
		auto message = beast::to_string(buffer.data());
		buffer.consume(buffer.size());
		return message;
	}

	void close()
	{
		ws.close(beast::websocket::close_code::normal);
	}
};

BOOST_AUTO_TEST_SUITE(SessionTests)
BOOST_AUTO_TEST_CASE(CreateSessionsAndGiveThemIds)
{
	boost::asio::io_service ios;
	SessionManager manager;
	boost::asio::ip::tcp::socket socket(ios);
	std::shared_ptr<Session> session1 = std::make_shared<Session>(manager, std::move(socket));
	BOOST_CHECK(session1->getId() == 0);
	std::shared_ptr<Session> session2 = std::make_shared<Session>(manager, std::move(socket));
	BOOST_CHECK(session2->getId() == 1);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ManagerTests)
BOOST_AUTO_TEST_CASE(RegisterSessions)
{
	boost::asio::io_service ios;
	SessionManager manager;
	boost::asio::ip::tcp::socket socket(ios);
	BOOST_CHECK(manager.connected() == 0);
	std::shared_ptr<Session> session1 = std::make_shared<Session>(manager, std::move(socket));
	manager.registerSession(session1);
	BOOST_CHECK(manager.connected() == 1);
	std::shared_ptr<Session> session2 = std::make_shared<Session>(manager, std::move(socket));
	manager.registerSession(session2);
	BOOST_CHECK(manager.connected() == 2);
}

BOOST_AUTO_TEST_CASE(UnregisterSessions)
{
	boost::asio::io_service ios;
	SessionManager manager;
	boost::asio::ip::tcp::socket socket(ios);
	std::shared_ptr<Session> session1 = std::make_shared<Session>(manager, std::move(socket));
	manager.registerSession(session1);
	std::shared_ptr<Session> session2 = std::make_shared<Session>(manager, std::move(socket));
	manager.registerSession(session2);
	BOOST_CHECK(manager.connected() == 2);
	manager.unregisterSession(session1->getId());
	BOOST_CHECK(manager.connected() == 1);
	manager.unregisterSession(session2->getId());
	BOOST_CHECK(manager.connected() == 0);
	BOOST_CHECK_THROW(manager.unregisterSession(session1->getId()), std::runtime_error);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ServerTests)
BOOST_AUTO_TEST_CASE(LaunchServerAndJoinWithClients)
{
	Server server;
	server.run("127.0.0.1", 2137);
	BOOST_CHECK(server.connected() == 0);
	Client client1;
	auto json = json::parse(client1.read().c_str());
	BOOST_CHECK(json["action"] == "welcome");
	BOOST_CHECK(server.connected() == 1);
	{
		Client client2;
		BOOST_CHECK(server.connected() == 2);
	}
	client1.write("{\"action\":\"show\"}");
	json = json::parse(client1.read().c_str());
	BOOST_CHECK(json["action"] == "show");
	BOOST_CHECK(server.connected() == 1);
}
BOOST_AUTO_TEST_SUITE_END()