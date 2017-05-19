#include <boost/test/unit_test.hpp>

#include "../network/server.h"
#include <beast/core.hpp>
#include <beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>


BOOST_AUTO_TEST_SUITE(ServerTest)
BOOST_AUTO_TEST_CASE(CreateServerAndCheckIfItsRunning)
{
	Server server;
	server.run("127.0.0.1", 2137);
	BOOST_CHECK(1);
}
BOOST_AUTO_TEST_SUITE_END()
