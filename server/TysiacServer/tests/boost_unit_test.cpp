#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE game_engine_test
#include <boost/test/unit_test.hpp>
#include "../engine/GameLogic.hpp"

BOOST_AUTO_TEST_SUITE( CardTests )
BOOST_AUTO_TEST_CASE( CreateCardAndGetMemberValues )
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
				c1.getSuit() == c2.getSuit() );
}

BOOST_AUTO_TEST_CASE(CreateCardAndSetIfUsed)
{
	Card c1(NINE, CLUBS);
	BOOST_REQUIRE(c1.getIsUsed() == false);
	c1.setIsUsed(true);
	BOOST_CHECK(c1.getIsUsed() == true);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( PlayerDeckTests )
BOOST_AUTO_TEST_CASE(AddCardCheckDeckLengthAndClearDeck)
{
	PlayerDeck p1;
	Card c1(NINE, CLUBS);
	p1.addCard(c1);
	BOOST_REQUIRE(p1.getDeck().size() == 1);
	p1.clearDeck();
	BOOST_CHECK(p1.getDeck().size() == 0);
}

BOOST_AUTO_TEST_CASE( CheckIfThereIsPairAndPlayCard )
{
	PlayerDeck p1;
	Card c1(NINE, CLUBS), c2(QUEEN, DIAMONDS), c3(KING, DIAMONDS);
	p1.addCard(c1);
	p1.addCard(c2);
	p1.addCard(c3);
	BOOST_REQUIRE(p1.playCard(1).getFigure() == c2.getFigure());
	BOOST_CHECK(p1.doesHavePair(DIAMONDS) == true);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( ScoreTests )
BOOST_AUTO_TEST_CASE(GetAndSetOverallScore)
{
	Score s1;
	BOOST_REQUIRE(s1.getScore() == 0);
	s1.addScore(200);
	BOOST_CHECK(s1.getScore() == 200);
}

BOOST_AUTO_TEST_CASE(GetAndSetTurnScore)
{
	Score s1;
	BOOST_REQUIRE(s1.getTurnScore() == 0);
	s1.addToTurnScore(15);
	BOOST_REQUIRE(s1.getTurnScore() == 15);
	s1.clearTurnScore();
	BOOST_CHECK(s1.getTurnScore() == 0);
}

BOOST_AUTO_TEST_CASE(GetAndSetClaim)
{
	Score s1;
	BOOST_REQUIRE(s1.getClaim() == 0);
	BOOST_REQUIRE(s1.setClaim(-10) == false);
	BOOST_REQUIRE(s1.setClaim(100) == true);
	BOOST_REQUIRE(s1.getClaim() == 100);
	s1.resetClaim();
	BOOST_CHECK(s1.getClaim() == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( PlayerTests )
BOOST_AUTO_TEST_CASE(CreatePlayerAndCheckItsParameters)
{
	std::string str = "test";
	Player p1(12, str);
	BOOST_CHECK(p1.getPlayersNick() == "test");
	BOOST_CHECK(p1.getPlayerId() == 12);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( DeckTests )
BOOST_AUTO_TEST_CASE(HandOutCards)
{
	Deck d1;
	std::string s = "test";
	Player p1(1, s);
	std::vector<Player> players;
	for (int i = 0; i < 3; ++i) {
		players.push_back(p1);
	}
	BOOST_REQUIRE(players[0].getPlayerDeck().getDeck().size() == 0);
	d1.dealCards(players);
	BOOST_REQUIRE(players[0].getPlayerDeck().getDeck().size() == 7);
	d1.addBonusCards(players[0]);
	BOOST_CHECK(players[0].getPlayerDeck().getDeck().size() == 10);
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
//
//BOOST_AUTO_TEST_CASE()
BOOST_AUTO_TEST_SUITE_END()

//BOOST_AUTO_TEST_SUITE_()