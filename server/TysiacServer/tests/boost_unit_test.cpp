#define BOOST_TEST_MODULE game_engine_test

#include <boost/test/unit_test.hpp>
#include "../engine/GameLogic.hpp"

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
	BOOST_CHECK_THROW(getPlayer(0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(SetNonExistingPlayer)
{
	BOOST_CHECK_THROW(setCurrentPlayer(0), std::out_of_range);
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
	PCroupier croupier = std::make_shared<Croupier>(0, man);
	Dealer d(deck, players);
	players.addPlayer(1, s);
	players.addPlayer(3, s);
	players.setHighestClaimer(players.getArray()[0]);
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
	PCroupier croupier = std::make_shared<Croupier>(0, man);
	Dealer d(deck, players);
	Bidder bid(deck, players);
	players.addPlayer(1, s);
	players.addPlayer(3, s);
	players.setCurrentPlayer(1);
	players.getNextPlayer();
	bid.Bid(3, 120);
	BOOST_REQUIRE((*players.getHighestClaimer()).getPlayerId() == 3);
	BOOST_CHECK((*players.getHighestClaimer()).getScoreClass().getClaim() == 120);
}
BOOST_AUTO_TEST_CASE(ThrowIfBidsNotAtHisTurn)
{
	GameManager man;
	Deck deck;
	PlayersCollection players;
	std::string s = "test";
	Player p(0, s);
	PCroupier croupier = std::make_shared<Croupier>(0, man);
	Dealer d(deck, players);
	Bidder bid(deck, players);
	players.addPlayer(1, s);
	players.addPlayer(3, s);
	players.getNextPlayer();
	BOOST_CHECK_THROW(bid.Bid(1, 110), std::logic_error);
}

BOOST_AUTO_TEST_CASE(ThrowIfBidsLessThanActual)
{
	GameManager man;
	Deck deck;
	PlayersCollection players;
	std::string s = "test";
	Player p(0, s);
	PCroupier croupier = std::make_shared<Croupier>(0, man);
	//croupier = std::make_shared<Croupier>(0, man, croupier);
	Dealer d(deck, players);
	Bidder bid(deck, players);
	players.addPlayer(1, s);
	players.addPlayer(3, s);
	players.getNextPlayer();
	BOOST_CHECK_THROW(bid.Bid(3, 90), std::logic_error);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AdderCroupierTests)
BOOST_AUTO_TEST_CASE(CroupierAddNewPlayer)
{
	json add_request = {
		{ "action", "add" }
		,{ "player" , 0 }
		,{ "values" , "test_nick" }
		,{ "id", 0 }
	};
	json expected_response;
	expected_response["who"] = 0;
	expected_response["type"] = "update";
	expected_response["action"] = "add";
	expected_response["id"] = 0;
	expected_response["player"] = 0;
	expected_response["values"] = "test_nick";
	std::string req_msg = expected_response.dump();
	std::string add_msg = add_request.dump();
	GameManager man;
	Croupier croupier(0, man);
	BOOST_CHECK_EQUAL(man.doWork(add_msg)[0].first, req_msg);
}
BOOST_AUTO_TEST_SUITE_END()
