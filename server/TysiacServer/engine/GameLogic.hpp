#ifndef _GAME_LOGIC_HPP_
#define _GAME_LOGIC_HPP_

#include <iostream>
#include <cstdio>
#include <utility>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <exception>
#include <json.hpp>

using json = nlohmann::json;
using request_type = std::vector<json>;
using req = std::vector<std::pair<std::string, std::vector<int> > >;

const int MAX_TURNS = 8;
const int MAX_PLAYERS = 3;
const int MAX_CARDS = 7;
const int TWO_CARDS = 2;
const int POINTS_WINNING_CAP = 1000;

enum commands {
	ADD,
	GET,
	BID,
	PLAY,
	CHAT
};

enum stage {
	ADDING,
	BIDDING,
	DEALING,
	PLAYING,
	SUMMING_UP,
	ENDING
};
enum figures {
	NOT_A_FIGURE = -1,
	NINE = 0,
	TEN = 10,
	JACK = 2,
	QUEEN = 3,
	KING = 4,
	ACE = 11
};
enum suits {
	NONE,
	DIAMONDS = 80,
	CLUBS = 60,
	HEARTS = 100,
	SPADES = 40
};
class Card {
public:
	Card(figures, suits);
	Card(const Card&);
	Card(Card&&);
	Card operator=(const Card&);
	Card operator=(Card&&);
	~Card();
	bool getIsUsed() const;
	void setIsUsed(bool) const;
	const figures getFigure() const;
	const suits getSuit() const ;
private:
	mutable bool is_used_;
	figures figure_;
	suits suit_;
};

class PlayerDeck {
public:
	PlayerDeck();
	~PlayerDeck();
	std::vector<Card> & getDeck();
	void clearDeck();
	void addCard(const Card&);
	const Card & playCard(int) const;
	bool doesHavePair(suits);
private:
	bool findCard(figures figure, suits suit) const;

	std::vector<Card> deck_;
};

class Score {
public:
	Score();
	~Score();
	int getScore() const;
	void addScore(int);
	void addToTurnScore(int);
	int getTurnScore() const;
	void clearTurnScore();
	bool setClaim(int);
	int getClaim() const;
	void resetClaim();
private:
	int score_;
	int temp_score_;
	int claim_;
};

class Player {
public:
	Player(int, std::string &);
	~Player();
	bool operator==(const Player &) const;
	PlayerDeck & getPlayerDeck();
	int getPlayerId() const;
	const std::string & getPlayersNick() const;
	Score & getScoreClass();
private:
	Score score_;
	int player_id_;
	const std::string nick_;
	PlayerDeck player_deck_;
};

class Deck {
public:
	Deck(const std::vector<Card>&);
	Deck(const Deck& other);
	Deck();
	~Deck();
	void dealCards(std::vector<Player>&);
	void addBonusCards(Player &);
	void shuffle();
private:
	std::random_device rd_;
	std::vector<Card> deck_;
	std::vector<Card>::iterator deck_it_;
};

class PlayersCollection {
public:
	PlayersCollection();
	PlayersCollection(const PlayersCollection &);
	~PlayersCollection();
	bool addPlayer(int, std::string &);
	bool getNextPlayer();
	void getNextCompulsoryClaimer();
	std::vector<Player> & getArray();
	std::vector<Player>::iterator & getCurrentPlayer();
	std::vector<Player>::iterator & getHighestClaimer();
	std::vector<Player>::iterator & getCompulsoryClaimer();
	void setHighestClaimer(Player &);
	bool setCurrentPlayer(int);
	Player getPlayer(int) const;

private:
	std::vector<Player> players_;
	std::vector<Player>::iterator highest_claimer_;
	std::vector<Player>::iterator players_it_;
	std::vector<Player>::iterator compulsory_claimer_;
};

class Tester {
public:
	Tester(Deck & deck, PlayersCollection & players);
	~Tester();
	void translateCard(const Card&) const;
	void doTests() const;
private:
	Deck  & deck_;
	PlayersCollection & players_;
};

class Adder;
class Bidder;
class Dealer;
class Game;
class Score;
class Croupier;
class GameManager;


class Adder {
public:
	Adder(Deck &, PlayersCollection &, Croupier &);
	~Adder();
	bool addPlayer(int, std::string);
	bool isFull() const;
private:
	Deck & deck_;
	PlayersCollection & players_;
	Croupier & croupier_;
};

class Bidder {
public:
	Bidder(Deck &, PlayersCollection &, Croupier &);
	~Bidder();
	bool Bid(int, int);
	void giveAddCards();
private:
	Deck & deck_;
	PlayersCollection & players_;
	Croupier & croupier_;
};
class Dealer {
public:
	Dealer(Deck &, PlayersCollection &, Croupier &);
	~Dealer();
	void giveCardToPeer(int player_id, int card_number);
	void reset();
private:
	int user_id_;
	int counter;
	Deck & deck_;
	PlayersCollection & players_;
	Croupier & croupier_;
};

class Game {
public:
	Game(Deck &, PlayersCollection &, Croupier &);
	~Game();
	auto playTurn(int, int);
	void manageTurn(int, int);
	int setSuperiorSuit();
	int compareCardsAndPassToWinner();
	void setStartingPlayer(int);
	void reset();
private:
	std::vector<std::pair<int, Card> > vec_;
	int turn_counter_;
	int current_starting_player_;
	suits super_suit_;
	Deck & deck_;
	PlayersCollection & players_;
	Croupier & croupier_;
};

class SumScore {
public:
	SumScore(Deck &, PlayersCollection &, Croupier &);
	~SumScore();
	void sumUpScore();
	void resetPlayersAtributes();
private:
	Deck & deck_;
	PlayersCollection & players_;
	Croupier & croupier_;
};

class GameManager {
public:
	GameManager();
	~GameManager();
	req doWork(const std::string &);
	void pushMessage(const request_type &);
private:
	std::vector<Croupier*> active_games_;
	request_type feedback_;
	unsigned croupier_counter_;
};

class Croupier {
public:
	Croupier(int, GameManager&);
	Croupier(const Croupier &);
	~Croupier();
	void changeStage(stage);
	bool runGame(const json &);
	int parse(const std::string &);
private:
	json chatMessage(const json & msg);
	GameManager & man_;
	int croupier_id_;
	Deck deck_;
	PlayersCollection players_;
	stage stage_;
	Adder adder_;
	Bidder bidder_;
	Dealer dealer_;
	Game game_;
	SumScore score_;
};






#endif // _GAME_LOGIC_HPP_

//->Dodawanie graczy
//->Licytacja
//->Musik, oddanie po karcie kolegom
//->Wygrywanie lew (+ kozyry i chwalonki czy jak im tam)
//->punktacja


