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

//BOOST_AUTO_TEST_SUITE( ScoreTests )
//BOOST_AUTO_TEST_CASE( CreateScore)