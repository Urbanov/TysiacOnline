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
class Room;
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
using PRoom = std::shared_ptr<Room>;
//using player_info = std::vector<std::pair<int, std::string> >;

const int MAX_TURNS = 8;
const int MAX_PLAYERS = 3;
const int MAX_CARDS = 7;
const int TWO_CARDS = 2;
const int MIN_RAISE = 10;
const int MIN_VALUE = 120;
const int POINTS_WINNING_CAP = 1000;
enum iterators {
	X = -1,
	CURRENT = 0,
	COMPULSORY = 1,
	HIGHEST = 2,
};

enum commands {
	LEAVE,
	DISCONNECT,
	READY,
	ADD,
	GET,
	BID,
	PLAY,
	CHAT
};

enum stage {
	FAIL,
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
	const suits getSuit() const;
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
	const Card & playCard(std::size_t) const;
	bool doesHavePair(suits);
	std::vector<int> getAllValidCards(std::vector<Card> &, suits);
	std::size_t getMaxValue();
private:
	bool findCard(figures figure, suits suit) const;
	bool isHigher(const Card &, const Card, suits);
	std::vector<Card> deck_;
	std::size_t max_value_;
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
	Player(const Player& other);
	Player & operator=(const Player & other);
	~Player();
	bool operator==(const Player &) const;
	PlayerDeck & getPlayerDeck();
	size_t getPlayerId() const;
	const std::string & getPlayersNick() const;
	Score & getScoreClass();
	bool getReady() const;
	void setReady(bool);
private:
	bool ready_;
	Score score_;
	int player_id_;
	std::string nick_;
	PlayerDeck player_deck_;
};

class Deck {
public:
	Deck(const std::vector<Card>&);
	Deck(const Deck& other);
	Deck();
	~Deck();
	void dealCards(players&);
	json addBonusCards(Player &);
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
	size_t getNextPlayer(iterators);
	request_type getPlayerInfo() const;
	players & getArray();
	Player & getPlayer(iterators, size_t = 0);
	void setPlayer(iterators, size_t);
	void prepareGame();
private:
	players players_;
	std::vector<size_t> it_;
};

class Controller {
public:
	Controller(Deck & deck, PlayersCollection & players);
	~Controller();

protected:
	Deck & deck_;
	PlayersCollection & players_;
};


class Adder : public Controller {
public:
	Adder(Deck &, PlayersCollection &);
	~Adder();
	stage addPlayer(int, std::string);
	stage setPlayerReady(int , bool);
	bool isFull() const;
};

class Bidder : public Controller {
public:
	Bidder(Deck &, PlayersCollection &);
	~Bidder();
	stage bid(int, int);
	void giveAddCards();
	request_type produceMessages(const json & msg, stage stage_);
};

class Dealer : public Controller {
public:
	Dealer(Deck &, PlayersCollection &);
	~Dealer();
	stage giveCardToPeer(int player_id, std::size_t card_number);
	std::vector<Card> showStockCards(size_t player_id);
	void reset();
private:
	int user_id_;
	int counter;
};

class Game : public Controller {
public:
	Game(Deck &, PlayersCollection &);
	~Game();
	const Card & playTurn(int, std::size_t);
	stage manageTurn(int, int);
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
	SumScore(Deck &, PlayersCollection &);
	~SumScore();
	stage sumUpScore();
	void resetPlayersAtributes();
};

class AddManager
{
public:
	AddManager(std::vector<PRoom> &);
	~AddManager();
	req addPlayer(const json &);
private:
	std::vector<PRoom> & active_games_;
};

class GameManager {
public:
	GameManager();
	~GameManager();
	req doWork(std::size_t, const std::string &);
	void pushMessage(const request_type &);
protected:
	void returnExistingRooms(const json &);
	void attachClientIdsToMessage();
	int findGameId(size_t) const;
	void addPlayer(json &);

	std::vector<std::vector<size_t> > players_;
	std::vector<PRoom> active_games_;
	request_type feedback_;
	req server_response_;
	std::size_t room_counter_;
};

class Room : public std::enable_shared_from_this<Room> {
public:
	Room(int, GameManager&);
	Room(const Room &);
	~Room();
	void changeStage(stage);
	bool runGame(const json &);
	int parse(const std::string &);
	bool isEmpty();
	size_t getRoomId() const;
	json getPlayersInfo() const;
private:
	json chatMessage(const json & msg);
	GameManager & man_;
	size_t room_id_;
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