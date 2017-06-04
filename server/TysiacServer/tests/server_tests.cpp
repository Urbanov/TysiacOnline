#include <boost/test/unit_test.hpp>

#include <beast/core.hpp>
#include <json.hpp>
#include "../network/server.h"

// simple sync client for server testing
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
	{}

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

	void connect()
	{
		boost::system::error_code error_code;
		boost::asio::connect(socket, resolver.resolve(boost::asio::ip::tcp::resolver::query{ "127.0.0.1", "2137" }), error_code);
		ws.handshake("127.0.0.1", "/", error_code);
	}

	void disconnect()
	{
		ws.close(beast::websocket::close_code::normal);
		ws.next_layer().close();
	}
};

BOOST_AUTO_TEST_SUITE(SessionTests)
BOOST_AUTO_TEST_CASE(CreateSessionsAndGiveThemIds)
{
	boost::asio::io_service ios;
	SessionManager manager;
	boost::asio::ip::tcp::socket socket(ios);

	// create 3 sessions
	std::shared_ptr<Session> session1 = std::make_shared<Session>(manager, std::move(socket));
	std::shared_ptr<Session> session2 = std::make_shared<Session>(manager, std::move(socket));
	std::shared_ptr<Session> session3 = std::make_shared<Session>(manager, std::move(socket));

	// chceck whether their IDs are correct
	size_t base_id = session1->getId();
	BOOST_CHECK_EQUAL(session2->getId(), base_id + 1);
	BOOST_CHECK_EQUAL(session3->getId(), base_id + 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ManagerTests)
BOOST_AUTO_TEST_CASE(RegisterSessions)
{
	boost::asio::io_service ios;
	SessionManager manager;
	boost::asio::ip::tcp::socket socket(ios);

	// no registered sessions
	BOOST_CHECK_EQUAL(manager.connected(), 0);
	// create a session
	std::shared_ptr<Session> session1 = std::make_shared<Session>(manager, std::move(socket));
	// register it
	manager.registerSession(session1);
	// check whether it is registered correctly
	BOOST_CHECK_EQUAL(manager.connected(), 1);
	// another one
	std::shared_ptr<Session> session2 = std::make_shared<Session>(manager, std::move(socket));
	// register...
	manager.registerSession(session2);
	// check...
	BOOST_CHECK_EQUAL(manager.connected(), 2);
}

BOOST_AUTO_TEST_CASE(UnregisterSessions)
{
	boost::asio::io_service ios;
	SessionManager manager;
	boost::asio::ip::tcp::socket socket(ios);

	// register two sessions
	std::shared_ptr<Session> session1 = std::make_shared<Session>(manager, std::move(socket));
	manager.registerSession(session1);
	std::shared_ptr<Session> session2 = std::make_shared<Session>(manager, std::move(socket));
	manager.registerSession(session2);
	BOOST_CHECK_EQUAL(manager.connected(), 2);

	// remove them one by one
	manager.unregisterSession(session1->getId());
	BOOST_CHECK_EQUAL(manager.connected(), 1);
	manager.unregisterSession(session2->getId());
	BOOST_CHECK_EQUAL(manager.connected(), 0);

	// try to remove not existing session
	BOOST_CHECK_THROW(manager.unregisterSession(session1->getId()), std::runtime_error);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ServerTests)
BOOST_AUTO_TEST_CASE(StartAndStopServer)
{
	// start a server and check if its accepting new connections
	Server server;
	server.run("127.0.0.1", 2137);
	BOOST_CHECK(server.isAccepting());

	// stop and check again
	server.stop();
	BOOST_CHECK(!server.isAccepting());
}

BOOST_AUTO_TEST_CASE(WriteAndReadWithWebsocket)
{
	Server server;
	server.run("127.0.0.1", 2137);

	// connect with one client and receive welcome message
	Client client1;
	client1.connect();
	auto json = json::parse(client1.read().c_str());
	
	// connect with another client
	Client client2;
	client2.connect();
	json = json::parse(client2.read().c_str());
	BOOST_CHECK(json["action"] == "welcome");

	// send a message to server
	json.clear();
	json["action"] = "show";
	client2.write(json.dump());

	// receive response
	json = json::parse(client2.read().c_str());
	BOOST_CHECK(json["action"] == "show");

	client1.disconnect();
	client2.disconnect();
}
BOOST_AUTO_TEST_SUITE_END()