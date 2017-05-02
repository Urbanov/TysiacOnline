#define BOOST_TEST_MODULE game_engine_test
#include <boost/test/unit_test.hpp>
#include "../engine/GameLogic.hpp"

class TestingManager : public GameManager{
	TestingManager();
	~TestingManager();
	
};

std::string createAddRequest(int server_id)
{
	json req0 = {
		 { "action", "add" }
		,{ "data" , "test_nick" }
		,{ "id", server_id }
	};
	return req0.dump();
}

std::string createAddAnswer(std::vector<int> server_id, bool is_add_or_new_player, bool if_failed)
{
	json res0;
	if (if_failed) {
		res0["error"] = true;
	}
	else {
		res0["error"] = false;
	}
	if (is_add_or_new_player) {
		res0["action"] = "add";
	}
	else {
		res0["action"] = "new_player";
		json tmp = {
			{"id", server_id[0]}
			,{"nick", "test_nick"}
		};
		res0["data"] = tmp;
		res0.erase("error");
		return res0.dump();
	}
	for (auto i : server_id) {
		json player = {
			{ "id", i }
			,{ "nick", "test_nick" }
		};
		res0["data"].push_back(player);
	}
	return res0.dump();
}

std::string createTextMessage(std::string& msg, int player_id = -1)
{
	json chat_message = {
		 {"action", "chat"}
		,{"data", msg }
	};
	if (player_id != -1) {
		chat_message["player"] = player_id;
	}
	return chat_message.dump();
}

std::string createReadyMessage(int player_id = -1)
{
	json ready = {
		{"action", "ready"}
	};
	if (player_id != -1) {
		ready["player"] = player_id;
	}
	return ready.dump();
}

BOOST_AUTO_TEST_SUITE(CardTests)
BOOST_AUTO_TEST_CASE(CreateCardAndGetMemberValues)
{
	Card c1(NINE, CLUBS);
	BOOST_CHECK(c1.getFigure() == NINE);
	BOOST_CHECK(c1.getSuit() == CLUBS);
}

BOOST_AUTO_TEST_CASE(CreateAnotherCardAndCheckIfIsEqual)
{
	Card c1(NINE, CLUBS);
	Card c2(c1);
	BOOST_CHECK(c1.getFigure() == c2.getFigure() &&
		c1.getSuit() == c2.getSuit());
}

BOOST_AUTO_TEST_CASE(CreateCardAndSetIfUsed)
{
	Card c1(NINE, CLUBS);
	BOOST_REQUIRE(c1.getIsUsed() == false);
	c1.setIsUsed(true);
	BOOST_CHECK(c1.getIsUsed() == true);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(PlayerDeckTests, PlayerDeck)
BOOST_AUTO_TEST_CASE(AddCardCheckDeckLengthAndClearDeck)
{
	Card c1(NINE, CLUBS);
	addCard(c1);
	BOOST_REQUIRE(getDeck().size() == 1);
	clearDeck();
	BOOST_CHECK(getDeck().size() == 0);
}

BOOST_AUTO_TEST_CASE(CheckIfThereIsPairAndPlayCard)
{
	Card c1(NINE, CLUBS), c2(QUEEN, DIAMONDS), c3(KING, DIAMONDS);
	addCard(c1);
	addCard(c2);
	addCard(c3);
	BOOST_REQUIRE(playCard(1).getFigure() == c2.getFigure());
	BOOST_CHECK(doesHavePair(DIAMONDS) == true);
}

BOOST_AUTO_TEST_CASE(ThrowWhenTryingToPlayNonExistingCard)
{
	BOOST_CHECK_THROW(playCard(0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(ThrowWhenTryingToPlaySameCardTwice)
{
	addCard(Card(NINE, CLUBS));
	playCard(0);
	BOOST_CHECK_THROW(playCard(0), std::logic_error);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ScoreTests, Score)
BOOST_AUTO_TEST_CASE(GetAndSetOverallScore)
{
	BOOST_REQUIRE(getScore() == 0);
	addScore(200);
	BOOST_CHECK(getScore() == 200);
}

BOOST_AUTO_TEST_CASE(GetAndSetTurnScore)
{
	BOOST_REQUIRE(getTurnScore() == 0);
	addToTurnScore(15);
	BOOST_REQUIRE(getTurnScore() == 15);
	clearTurnScore();
	BOOST_CHECK(getTurnScore() == 0);
}

BOOST_AUTO_TEST_CASE(GetAndSetClaim)
{
	BOOST_REQUIRE(getClaim() == 0);
	BOOST_CHECK_THROW(setClaim(-10), std::logic_error);
	BOOST_REQUIRE(setClaim(100) == true);
	BOOST_REQUIRE(getClaim() == 100);
	resetClaim();
	BOOST_CHECK(getClaim() == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PlayerTests)
BOOST_AUTO_TEST_CASE(CreatePlayerAndCheckItsParameters)
{
	std::string str = "test";
	Player p1(12, str);
	BOOST_CHECK(p1.getPlayersNick() == "test");
	BOOST_CHECK(p1.getPlayerId() == 12);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(DeckTests, Deck)
BOOST_AUTO_TEST_CASE(HandOutCards)
{
	std::string s = "test";
	Player p1(1, s);
	players players_array;
	for (int i = 0; i < 3; ++i) {
		players_array.push_back(p1);
	}
	BOOST_REQUIRE(players_array[0].getPlayerDeck().getDeck().size() == 0);
	dealCards(players_array);
	BOOST_REQUIRE(players_array[0].getPlayerDeck().getDeck().size() == 7);
	addBonusCards(players_array[0]);
	BOOST_CHECK(players_array[0].getPlayerDeck().getDeck().size() == 10);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(PlayersCollectionTest, PlayersCollection)
BOOST_AUTO_TEST_CASE(AddNewPlayer)
{
	std::string s = "test";
	BOOST_REQUIRE(getArray().size() == 0);
	addPlayer(12, s);
	BOOST_CHECK(getArray().size() == 1);
}

BOOST_AUTO_TEST_CASE(GetNotExistingPlayer)
{
	BOOST_CHECK_THROW(getPlayer(X, 0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(SetNonExistingPlayer)
{
	BOOST_CHECK_THROW(setPlayer(CURRENT,0), std::out_of_range);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(DealerTest)
BOOST_AUTO_TEST_CASE(DealerGiveCardToPeer)
{
	GameManager man;
	Deck deck;
	PlayersCollection players;
	std::string s = "test";
	Player p(0, s);
	PRoom croupier = std::make_shared<Room>(0, man);
	Dealer d(deck, players);
	players.addPlayer(1, s);
	players.addPlayer(3, s);
	players.addPlayer(4, s);
	players.prepareGame();
	players.setPlayer(HIGHEST, players.getArray()[0].getPlayerId());
	BOOST_CHECK_THROW(d.giveCardToPeer(2, 0), std::out_of_range);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(BidderTest)
BOOST_AUTO_TEST_CASE(IfBidsBecomesHighestBidder)
{
	GameManager man;
	Deck deck;
	PlayersCollection players;
	std::string s = "test";
	PRoom croupier = std::make_shared<Room>(0, man);
	Dealer d(deck, players);
	Bidder bid(deck, players);
	players.addPlayer(1, s);
	players.addPlayer(3, s);
	players.addPlayer(4, s);
	players.prepareGame();
	bid.Bid(3, 120);
	BOOST_CHECK_EQUAL(3, players.getPlayer(HIGHEST).getPlayerId());
	BOOST_CHECK_EQUAL(players.getPlayer(HIGHEST).getScoreClass().getClaim(),120);
}
BOOST_AUTO_TEST_CASE(ThrowIfBidsNotAtHisTurn)
{
	GameManager man;
	Deck deck;
	PlayersCollection players;
	std::string s = "test";
	Player p(0, s);
	PRoom croupier = std::make_shared<Room>(0, man);
	Dealer d(deck, players);
	Bidder bid(deck, players);
	players.addPlayer(1, s);
	players.addPlayer(3, s);
	players.addPlayer(4, s);
	players.prepareGame();
	players.getNextPlayer(CURRENT);
	BOOST_CHECK_THROW(bid.Bid(1, 110), std::logic_error);
}

BOOST_AUTO_TEST_CASE(ThrowIfBidsLessThanActual)
{
	GameManager man;
	Deck deck;
	PlayersCollection players;
	std::string s = "test";
	Player p(0, s);
	PRoom croupier = std::make_shared<Room>(0, man);
	Dealer d(deck, players);
	Bidder bid(deck, players);
	players.addPlayer(1, s);
	players.addPlayer(3, s);
	players.addPlayer(4, s);
	players.prepareGame();
	players.getNextPlayer(CURRENT);
	BOOST_CHECK_THROW(bid.Bid(3, 90), std::logic_error);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AdderRoomTests)
BOOST_AUTO_TEST_CASE(RoomAddNewPlayer)
{
	GameManager man;
	BOOST_CHECK_EQUAL(man.doWork(0, createAddRequest(-1))[0].first, createAddAnswer({ 0 }, true, false));
}

BOOST_AUTO_TEST_CASE(RoomAddAtLeastFourPlayersSoMultipleRoomsAreCreated)
{
	GameManager man;
	BOOST_CHECK_EQUAL(man.doWork(0, createAddRequest(-1))[0].first, createAddAnswer({ 0 }, true, false));
	BOOST_CHECK_EQUAL(man.doWork(1, createAddRequest(0))[0].first, createAddAnswer({ 0, 1 }, true, false));
	BOOST_CHECK_EQUAL(man.doWork(2, createAddRequest(0))[0].first, createAddAnswer({ 0, 1, 2 }, true, false));
	BOOST_CHECK_EQUAL(man.doWork(3, createAddRequest(-1))[0].first, createAddAnswer({ 3 }, true, false));
}

BOOST_AUTO_TEST_CASE(AddToRoomMoreThan3)
{
	GameManager man;
	BOOST_CHECK_EQUAL(man.doWork(0, createAddRequest(-1))[0].first, createAddAnswer({ 0 }, true, false));
	BOOST_CHECK_EQUAL(man.doWork(1, createAddRequest(0))[0].first, createAddAnswer({ 0, 1 }, true, false));
	BOOST_CHECK_EQUAL(man.doWork(2, createAddRequest(0))[0].first, createAddAnswer({ 0, 1, 2 }, true, false));
	BOOST_CHECK_EQUAL(man.doWork(3, createAddRequest(0))[0].first, createAddAnswer({ }, true, true));
}


BOOST_AUTO_TEST_CASE(AddTwoAndGetAllMessageBack)
{
	GameManager man;
	man.doWork(0, createAddRequest(-1));
	req feedback2 = man.doWork(1, createAddRequest(0));
	BOOST_CHECK_EQUAL(feedback2[0].first, createAddAnswer({ 0, 1 }, true, false));
	BOOST_CHECK_EQUAL(feedback2[1].first, createAddAnswer({ 1 }, false, false));
}

BOOST_AUTO_TEST_CASE(SendAndReceiveMessage)
{
	GameManager man;
	std::string test = "test message";
	man.doWork(0, createAddRequest(-1));
	man.doWork(1, createAddRequest(0));
	man.doWork(2, createAddRequest(0));
	req feedback = man.doWork(1, createTextMessage(test));
	BOOST_CHECK_EQUAL(feedback.size(), 1);
	BOOST_CHECK_EQUAL(feedback[0].first, createTextMessage(test, 1));
	BOOST_CHECK_EQUAL(feedback[0].second[0], 0);
	BOOST_CHECK_EQUAL(feedback[0].second[1], 2);
}

BOOST_AUTO_TEST_CASE(PlayersGetReadyAndGetMessageBack)
{
	GameManager man;
	man.doWork(0, createAddRequest(-1));
	man.doWork(1, createAddRequest(0));
	man.doWork(2, createAddRequest(0));
	req feedback = man.doWork(0, createReadyMessage());
	BOOST_CHECK_EQUAL(feedback[0].first, createReadyMessage(0));
	BOOST_CHECK_EQUAL(feedback[0].second[0], 1);
	BOOST_CHECK_EQUAL(feedback[0].second[1], 2);
}

BOOST_AUTO_TEST_CASE(PlayersGetReadyAndBid)
{

}
BOOST_AUTO_TEST_SUITE_END()

