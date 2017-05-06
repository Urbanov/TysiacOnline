#define BOOST_TEST_MODULE game_engine_test
#include <boost/test/unit_test.hpp>
#include "../engine/GameLogic.hpp"

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

std::string createBidMessage(int bid)
{
	json msg = {
		{"action", "bid"},
		{"data", bid}
	};
	return msg.dump();
}

std::string createBidAnswer(size_t prev_id, int value, int curr_id)
{
	json msg = {
		{"action", "bid"},
		{"data", {{"value", value},{ "id", prev_id } }},
		{"player", curr_id}
	};
	return msg.dump();
}

std::string createDealMessage(int p1, int c1, int p2, int c2)
{
	json msg = {
		{"action", "deal"},
		{ "action", {{{"player", p1}, {"card", c1 } } , { { "player", p2 },{ "card", c2 } }} }
	};
	return msg.dump();
}

std::string createDealAnswer(suits suit, figures figure)
{
	json msg = {
		{"action", "deal"},
		{"data", {{"figure", figure}, {"suit", suit}} }
	};
	return msg.dump();
}

Player createPlayerWithAppropriateCards(suits type)
{
	std::string str = "test";
	Player player(type, str);
	std::vector<figures> figures = { NINE, JACK, QUEEN, KING, TEN, ACE };
	for (const auto& i : figures) {
		player.getPlayerDeck().addCard(Card(i, type));
	}
	return player;
}

void addPlayers(PlayersCollection& players)
{
	std::string test = "test";
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		players.addPlayer(i, test);
	}
}

void addScoreToPlayers(PlayersCollection & players, std::vector<std::pair<int, int> > vec)
{
	addPlayers(players);
	players.prepareGame();
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		players.getArray()[i].getScoreClass().setClaim(vec[i].first, false);
		players.getArray()[i].getScoreClass().addToTurnScore(vec[i].second);
	}
}

std::vector<int> getScoreOfPlayers(PlayersCollection & players)
{
	std::vector<int> vec;
	for (auto & i : players.getArray()) {
		vec.push_back(i.getScoreClass().getScore());
	}
	return vec;
}

void addCards(Player & player, std::vector<Card> vec)
{
	for (const auto & i : vec) {
		player.getPlayerDeck().addCard(i);
	}
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

BOOST_AUTO_TEST_CASE(ReturnAllCardsThatCanBePlayed1)
{
	std::vector<suits> suit = { CLUBS, SPADES, DIAMONDS, HEARTS };
	std::vector<figures> figure = { NINE, TEN, QUEEN, KING, ACE };
	for (const auto& i : suit) {
		for (const auto & j : figure) {
			addCard(Card(j, i));
		}
	}
	std::vector<Card> vec = { {JACK, SPADES}, {JACK, DIAMONDS} };
	json tmp = { 6, 7, 8, 9 };
	json tmp1 = getAllValidCards(vec, HEARTS);
	BOOST_CHECK_EQUAL(tmp1, tmp);

}

BOOST_AUTO_TEST_CASE(ReturnAllCardsThatCanBePlayed2)
{
	std::vector<suits> suit = { CLUBS, SPADES, DIAMONDS, HEARTS };
	std::vector<figures> figure = { NINE, TEN, QUEEN, KING, ACE };
	for (const auto& i : suit) {
		for (const auto & j : figure) {
			addCard(Card(j, i));
		}
	}
	std::vector<Card> vec = { { JACK, SPADES } };
	json tmp = { 6, 7, 8, 9 };
	json tmp1 = getAllValidCards(vec, HEARTS);
	BOOST_CHECK_EQUAL(tmp1, tmp);
}

BOOST_AUTO_TEST_CASE(ReturnAllCardsThatCanBePlayed3)
{
	std::vector<suits> suit = { CLUBS, SPADES, DIAMONDS, HEARTS };
	std::vector<figures> figure = { NINE, TEN, QUEEN, KING, ACE };
	for (const auto& i : suit) {
		for (const auto & j : figure) {
			addCard(Card(j, i));
		}
	}
	std::vector<Card> vec = { { JACK, SPADES }, { NINE, HEARTS } };
	json tmp = { 5, 6, 7, 8, 9 };
	json tmp1 = getAllValidCards(vec, HEARTS);
	BOOST_CHECK_EQUAL(tmp1, tmp);
}

BOOST_AUTO_TEST_CASE(ReturnAllCardsThatCanBePlayed4)
{
	std::vector<suits> suit = { CLUBS, DIAMONDS, HEARTS };
	std::vector<figures> figure = { NINE, TEN, QUEEN, KING, ACE };
	for (const auto& i : suit) {
		for (const auto & j : figure) {
			addCard(Card(j, i));
		}
	}
	std::vector<Card> vec = { { JACK, SPADES },{ NINE, SPADES } };
	json tmp = { 10, 11, 12, 13, 14 };
	json tmp1 = getAllValidCards(vec, HEARTS);
	BOOST_CHECK_EQUAL(tmp1, tmp);
}

BOOST_AUTO_TEST_CASE(ReturnAllCardsThatCanBePlayed5)
{
	std::vector<suits> suit = { CLUBS, SPADES, DIAMONDS, HEARTS };
	std::vector<figures> figure = { NINE, TEN, QUEEN, ACE };
	for (const auto& i : suit) {
		for (const auto & j : figure) {
			addCard(Card(j, i));
		}
	}
	std::vector<Card> vec = { { JACK, SPADES },{ TEN, SPADES } };
	json tmp = { 7 };
	json tmp1 = getAllValidCards(vec, HEARTS);
	BOOST_CHECK_EQUAL(tmp1, tmp);
}

BOOST_AUTO_TEST_CASE(ReturnAllCardsThatCanBePlayed6)
{
	std::vector<suits> suit = { CLUBS, SPADES, DIAMONDS, HEARTS };
	std::vector<figures> figure = { NINE, TEN, QUEEN, ACE };
	for (const auto& i : suit) {
		for (const auto & j : figure) {
			addCard(Card(j, i));
		}
	}
	std::vector<Card> vec = {};
	json tmp = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	json tmp1 = getAllValidCards(vec, HEARTS);
	BOOST_CHECK_EQUAL(tmp1, tmp);
}

BOOST_AUTO_TEST_CASE(ReturnAllCardsThatCanBePlayed7)
{
	std::vector<suits> suit = { CLUBS };
	std::vector<figures> figure = { NINE, JACK, QUEEN, KING, TEN, ACE };
	for (const auto& i : suit) {
		for (const auto & j : figure) {
			addCard(Card(j, i));
		}
	}
	std::vector<Card> vec = { {NINE, DIAMONDS}, {TEN, SPADES} };
	json tmp = { 0,1,2,3,4,5 };
	json tmp1 = getAllValidCards(vec, HEARTS);
	BOOST_CHECK_EQUAL(tmp1, tmp);
}

BOOST_AUTO_TEST_CASE(ReturnAllCardsThatCanBePlayed8)
{
	std::vector<suits> suit = { CLUBS };
	std::vector<figures> figure = { NINE, JACK, QUEEN, KING, TEN, ACE };
	for (const auto& i : suit) {
		for (const auto & j : figure) {
			addCard(Card(j, i));
		}
	}
	std::vector<Card> vec = { { NINE, DIAMONDS } };
	json tmp = { 0,1,2,3,4,5 };
	json tmp1 = getAllValidCards(vec, HEARTS);
	BOOST_CHECK_EQUAL(tmp1, tmp);
}

BOOST_AUTO_TEST_CASE(ReturnAllCardsThatCanBePlayed9)
{
	std::vector<suits> suit = { CLUBS };
	std::vector<figures> figure = { NINE, JACK, QUEEN, KING, TEN, ACE };
	for (const auto& i : suit) {
		for (const auto & j : figure) {
			addCard(Card(j, i));
		}
	}
	addCard(Card(NINE, DIAMONDS));
	getDeck().back().setIsUsed(true);
	std::vector<Card> vec = { { TEN, DIAMONDS } };
	json tmp = { 0,1,2,3,4,5 };
	json tmp1 = getAllValidCards(vec, HEARTS);
	BOOST_CHECK_EQUAL(tmp1, tmp);
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
	BOOST_CHECK_THROW(setClaim(-10, false), std::logic_error);
	BOOST_REQUIRE(setClaim(100, false) == true);
	BOOST_REQUIRE(getClaim() == 100);
	resetClaim();
	BOOST_CHECK(getClaim() == 0);
}

BOOST_AUTO_TEST_CASE(CheckIfRoundMethodWorksProperly1)
{
	int x = 3;
	BOOST_CHECK_EQUAL(round(x), 0);
}

BOOST_AUTO_TEST_CASE(CheckIfRoundMethodWorksProperly2)
{
	int x = 5;
	BOOST_CHECK_EQUAL(round(x), 10);
}

BOOST_AUTO_TEST_CASE(CheckIfRoundMethodWorksProperly3)
{
	int x = 10;
	BOOST_CHECK_EQUAL(round(x), 10);
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
	Deck deck;
	PlayersCollection players;
	Dealer d(deck, players);
	addPlayers(players);
	players.prepareGame();
	players.setPlayer(HIGHEST, players.getArray()[0].getPlayerId());
	BOOST_CHECK_THROW(d.giveCardToPeer(2, 0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(Return120MaxValue)
{
	std::string test = "test";
	Player p(0, test);
	addCards(p, { { QUEEN, HEARTS }, { QUEEN, DIAMONDS}, {KING, SPADES}, { QUEEN, CLUBS } });
	BOOST_CHECK_EQUAL(p.getPlayerDeck().getMaxValue(false), 120);
}

BOOST_AUTO_TEST_CASE(Return160MaxValue)
{
	std::string test = "test";
	Player p(0, test);
	addCards(p, { { QUEEN, HEARTS },{ QUEEN, DIAMONDS },{ KING, SPADES },{ QUEEN, SPADES } });
	BOOST_CHECK_EQUAL(p.getPlayerDeck().getMaxValue(false), 160);
}

BOOST_AUTO_TEST_CASE(Return180MaxValue)
{
	std::string test = "test";
	Player p(0, test);
	addCards(p, { { QUEEN, HEARTS },{ QUEEN, DIAMONDS },{ KING, SPADES },{ ACE, SPADES },
	{ KING, CLUBS },{ QUEEN, CLUBS} });
	BOOST_CHECK_EQUAL(p.getPlayerDeck().getMaxValue(false), 180);
}

BOOST_AUTO_TEST_CASE(Return200MaxValue)
{
	std::string test = "test";
	Player p(0, test);
	addCards(p, { { QUEEN, HEARTS },{ QUEEN, DIAMONDS },{ KING, SPADES },{ ACE, SPADES },
	{ KING, CLUBS },{ KING, DIAMONDS } });
	BOOST_CHECK_EQUAL(p.getPlayerDeck().getMaxValue(false), 200);
}

BOOST_AUTO_TEST_CASE(Return220MaxValue)
{
	std::string test = "test";
	Player p(0, test);
	addCards(p, { { QUEEN, HEARTS },{ QUEEN, DIAMONDS },{ KING, SPADES },{ ACE, SPADES },
	{ KING, HEARTS },{ QUEEN, CLUBS } });
	BOOST_CHECK_EQUAL(p.getPlayerDeck().getMaxValue(false), 220);
}

BOOST_AUTO_TEST_CASE(Return300MaxValue)
{
	std::string test = "test";
	Player p(0, test);
	addCards(p, { { QUEEN, HEARTS },{ QUEEN, DIAMONDS },{ KING, DIAMONDS },{ ACE, SPADES },
	{ KING, HEARTS },{ QUEEN, CLUBS } });
	BOOST_CHECK_EQUAL(p.getPlayerDeck().getMaxValue(false), 300);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(BidderTest)
BOOST_AUTO_TEST_CASE(IfBidsBecomesHighestBidder)
{
	Deck deck;
	PlayersCollection players;
	Bidder bid(deck, players);
	addPlayers(players);
	players.prepareGame();
	bid.bid(1, 120);
	BOOST_CHECK_EQUAL(1, players.getPlayer(HIGHEST).getPlayerId());
	BOOST_CHECK_EQUAL(players.getPlayer(HIGHEST).getScoreClass().getClaim(),120);
}
BOOST_AUTO_TEST_CASE(ThrowIfBidsNotAtHisTurn)
{
	Deck deck;
	PlayersCollection players;
	Bidder bid(deck, players);
	addPlayers(players);
	players.prepareGame();
	players.getNextPlayer(CURRENT);
	BOOST_CHECK_THROW(bid.bid(0, 110), std::logic_error);
}

BOOST_AUTO_TEST_CASE(ThrowIfBidsLessThanActual)
{
	Deck deck;
	PlayersCollection players;
	Bidder bid(deck, players);
	addPlayers(players);
	players.prepareGame();
	players.getNextPlayer(CURRENT);
	BOOST_CHECK_THROW(bid.bid(3, 90), std::logic_error);
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
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GameManagerTests)
BOOST_AUTO_TEST_CASE(SendAndReceiveMessage)
{
	GameManager man;
	std::string test = "test message";
	for (int i = 0; i < 3; ++i) {
		man.doWork(i, createAddRequest(i > 0 ? 0 : -1));
	}
	req feedback = man.doWork(1, createTextMessage(test));
	BOOST_CHECK_EQUAL(feedback.size(), 1);
	BOOST_CHECK_EQUAL(feedback[0].first, createTextMessage(test, 1));
	BOOST_CHECK_EQUAL(feedback[0].second[0], 0);
	BOOST_CHECK_EQUAL(feedback[0].second[1], 2);
}

BOOST_AUTO_TEST_CASE(PlayersGetReadyAndGetMessageBack)
{
	GameManager man;
	for (int i = 0; i < 3; ++i) {
		man.doWork(i, createAddRequest(i > 0 ? 0 : -1));
	}
	req feedback = man.doWork(0, createReadyMessage());
	BOOST_CHECK_EQUAL(feedback[0].first, createReadyMessage(0));
	BOOST_CHECK_EQUAL(feedback[0].second[0], 1);
	BOOST_CHECK_EQUAL(feedback[0].second[1], 2);
}

BOOST_AUTO_TEST_CASE(PlayersGetReadyAndBid)
{
	GameManager man;
	for (int i = 0; i < 3; ++i) {
		man.doWork(i, createAddRequest(i > 0 ? 0 : -1));
	}
	for (int i = 0; i < 3; ++i) {
		man.doWork(i, createReadyMessage());
	}
	req feedback = man.doWork(1, createBidMessage(110));
	BOOST_CHECK_EQUAL(feedback[0].first, createBidAnswer(1, 110, 2));
	json info = json::parse(feedback[1].first.begin(), feedback[1].first.end());
	BOOST_CHECK(info["data"]["min"] == 120);
	BOOST_CHECK(info["data"]["max"] != 0);
	BOOST_CHECK_EQUAL(feedback[1].second.size(), 1);
	BOOST_CHECK_EQUAL(feedback[1].second[0], 2);
}

BOOST_AUTO_TEST_CASE(PlayersGetReadyAndBidNegative)
{
	GameManager man;
	for (int i = 0; i < 3; ++i) {
		man.doWork(i, createAddRequest(i > 0 ? 0 : -1));
	}
	for (int i = 0; i < 3; ++i) {
		man.doWork(i, createReadyMessage());
	}
	req feedback = man.doWork(1, createBidMessage(-1));
	BOOST_CHECK_EQUAL(feedback[0].first, createBidAnswer(1, -1, 2));
	feedback = man.doWork(2, createBidMessage(-1));
	BOOST_CHECK_EQUAL(feedback[0].first, createBidAnswer(2, -1, -1));
	json msg = json::parse(feedback[1].first.begin(), feedback[1].first.end());
	BOOST_CHECK_EQUAL(msg["action"], "stock");
	BOOST_CHECK_EQUAL(msg["data"].size(), 3);
	BOOST_CHECK_EQUAL(feedback[1].second.size(), 3);
}

BOOST_AUTO_TEST_CASE(PlayThreeCardsAndGetCorrectMessageBack1)
{
	PlayersCollection players;
	Deck deck;
	Game game(deck, players);
	players.getArray().push_back(createPlayerWithAppropriateCards(SPADES));
	players.getArray().push_back(createPlayerWithAppropriateCards(DIAMONDS));
	players.getArray().push_back(createPlayerWithAppropriateCards(HEARTS));
	players.prepareGame();
	players.getNextPlayer(CURRENT);
	players.getNextPlayer(CURRENT);
	game.manageTurn(SPADES, 0);
	game.manageTurn(DIAMONDS, 0);
	game.manageTurn(HEARTS, 0);
	int player = game.compareCardsAndPassToWinner();
	BOOST_CHECK_EQUAL(player, SPADES);
}

BOOST_AUTO_TEST_CASE(PlayThreeCardsAndGetCorrectMessageBack2)
{
	PlayersCollection players;
	Deck deck;
	Game game(deck, players);
	players.getArray().push_back(createPlayerWithAppropriateCards(SPADES));
	players.getArray().push_back(createPlayerWithAppropriateCards(DIAMONDS));
	players.getArray().push_back(createPlayerWithAppropriateCards(HEARTS));
	players.getArray()[1].getPlayerDeck().addCard(Card(ACE, SPADES));
	players.prepareGame();
	players.getNextPlayer(CURRENT);
	players.getNextPlayer(CURRENT);
	game.manageTurn(SPADES, 2);
	game.manageTurn(DIAMONDS, 0);
	game.manageTurn(HEARTS, 0);
	request_type request = game.createMessages(PLAYING);
	json available_cards = request[0]["data"]["available"];
	json expected_cards = { 0,1,3,4,5 };
	BOOST_CHECK_EQUAL(available_cards, expected_cards);
	game.manageTurn(SPADES, 3);
	game.manageTurn(DIAMONDS, 6);
	game.manageTurn(HEARTS, 2);
	int player = game.compareCardsAndPassToWinner();
	BOOST_CHECK_EQUAL(player, DIAMONDS);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SumTestClassTests)
BOOST_AUTO_TEST_CASE(AddScoreToAllPlayers1)
{
	PlayersCollection players;
	Deck deck;
	SumScore score(deck, players);
	addScoreToPlayers(players, { { 140, 120 },{ -1, 36 },{ -1, 14 } });
	score.sumUpScore();
	json expected = { -140, 40, 10 }, returned = getScoreOfPlayers(players);
	BOOST_CHECK_EQUAL(expected, returned);
}

BOOST_AUTO_TEST_CASE(AddScoreToAllPlayers2)
{
	PlayersCollection players;
	Deck deck;
	SumScore score(deck, players);
	addScoreToPlayers(players, { { 140, 144 },{ -1, 36 },{ -1, 14 } });
	score.sumUpScore();
	json expected = { 140, 40, 10 }, returned = getScoreOfPlayers(players);
	BOOST_CHECK_EQUAL(expected, returned);
}

BOOST_AUTO_TEST_SUITE_END()