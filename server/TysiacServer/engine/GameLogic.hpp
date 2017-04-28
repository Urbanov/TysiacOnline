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

class Adder;
class Bidder;
class Card;
class Croupier;
class Dealer;
class Deck;
class Game;
class GameManager;
class Player;
class PlayersCollection;
class PlayerDeck;
class Score;
class SumScore;


using json = nlohmann::json;
using request_type = std::vector<json>;
using req = std::vector<std::pair<std::string, std::vector<int> > >;
using players = std::vector<Player>;
using players_it = std::vector<Player>::iterator;

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
	const Card & playCard(unsigned) const;
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
	void dealCards(players&);
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
	void setHighestClaimer(Player &);
	players & getArray();
	players_it & getCurrentPlayer();
	players_it & getHighestClaimer();
	players_it & getCompulsoryClaimer();
	players_it & setCurrentPlayer(int);
	Player & getPlayer(int);

private:
	players players_;
	players_it highest_claimer_;
	players_it players_it_;
	players_it compulsory_claimer_;
};

class Controller {
public:
	Controller(Deck & deck, PlayersCollection & players, Croupier & croup);
	~Controller();

protected:
	Deck & deck_;
	PlayersCollection & players_;
	Croupier & croupier_;
};


class Adder : public Controller {
public:
	Adder(Deck &, PlayersCollection &, Croupier &);
	~Adder();
	bool addPlayer(int, std::string);
	bool isFull() const;
};

class Bidder : public Controller{
public:
	Bidder(Deck &, PlayersCollection &, Croupier &);
	~Bidder();
	bool Bid(int, int);
	void giveAddCards();
};

class Dealer : public Controller {
public:
	Dealer(Deck &, PlayersCollection &, Croupier &);
	~Dealer();
	void giveCardToPeer(int player_id, unsigned card_number);
	void reset();
private:
	int user_id_;
	int counter;
};

class Game : public Controller {
public:
	Game(Deck &, PlayersCollection &, Croupier &);
	~Game();
	const Card & playTurn(int, unsigned);
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
};

class SumScore : public Controller {
public:
	SumScore(Deck &, PlayersCollection &, Croupier &);
	~SumScore();
	void sumUpScore();
	void resetPlayersAtributes();
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


