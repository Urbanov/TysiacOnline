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
#include <cmath>

class Adder;
class Bidder;
class Card;
class Room;
class Dealer;
class Deck;
class Game;
class GameManager;
class LeaveBuster;
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

const int MAX_TURNS = 8;
const int MAX_PLAYERS = 3;
const int MAX_CARDS = 7;
const int TWO_CARDS = 2;
const int MIN_RAISE = 10;
const int MIN_VALUE = 120;
const int POINTS_WINNING_CAP = 1000;
const int NO_TAX_CAP = 800;

enum iterators {
	X = -1,
	CURRENT = 0,
	COMPULSORY = 1,
	HIGHEST = 2,
};

enum commands {
	LEAVE = 0,
	DISCONNECT = 15,
	READY = 12,
	ADD = 2,
	DEAL = 4,
	BID = 3,
	PLAY = 5,
	CHAT = 1
};

enum stage {
	FAIL,
	LEAVING,
	ADDING,
	STARTING,
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
	NONE = 0,
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
	bool operator==(const Card& other);
	~Card();
	bool getIsUsed() const;
	void setIsUsed(bool) const;
	const figures getFigure() const;
	const suits getSuit() const;
	const Card & isBigger(const Card & other, suits suit);
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
	std::size_t getMaxValue(bool);
	bool isHigher(const Card &, const Card, suits);
private:
	bool findCard(figures figure, suits suit) const;
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
	bool setClaim(int, bool);
	int getClaim() const;
	void reset(bool);
	int round(int) const;
	void roundScore();
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
	void reset();
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
	void prepareGame(bool);
	void resetPlayerAttributes(bool);
private:
	players players_;
	std::vector<size_t> it_;
};

class Controller {
public:
	Controller(Deck & deck, PlayersCollection & players);
	virtual ~Controller();
	virtual stage changeModel(const json & msg, const stage stage_) = 0;
	virtual request_type createMessages(const json & msg, const stage stage_) = 0;
protected:
	Deck & deck_;
	PlayersCollection & players_;
};

class LeaveBuster : public Controller {
public:
	LeaveBuster(Deck &, PlayersCollection &);
	virtual ~LeaveBuster();
	virtual stage changeModel(const json & msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
};

class ChatBox : public Controller {
public:
	ChatBox(Deck&, PlayersCollection&);
	virtual ~ChatBox();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
};

class Adder : public Controller {
public:
	Adder(Deck &, PlayersCollection &);
	~Adder();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	bool isFull() const;

private:
	json acceptNewPlayer(const json & msg, stage stage_);
	request_type informOtherPlayers(const json & msg, stage stage_);
};

class Starter : public Controller {
public:
	Starter(Deck &, PlayersCollection &);
	virtual ~Starter();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	json createStartMessage(const json & msg) const;
	bool getIsFull() const;
	void setIsFull(bool);
private:
	bool isReadyToStart() const;
	void prepareToStart(stage);
	bool is_full_;
};

class Bidder : public Controller {
public:
	Bidder(Deck &, PlayersCollection &);
	~Bidder();

	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	stage bid(const json & msg, const stage stage_);
private:
	request_type createUpdateInfo(const json & msg) const;
	request_type createSpecialInfo(const json & msg) const;
	json createStock(const json & msg) const;
	request_type createBid(const json & msg, stage stage_) const;
	request_type firstBid(stage stage_);
	request_type createCardDealingMessages() const;
	request_type createStarterMessages(const json & msg, stage stage_);

	mutable json additional_cards_;
	Starter starter_;
};

class Dealer : public Controller {
public:
	Dealer(Deck &, PlayersCollection &);
	~Dealer();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	stage giveCardToPeer(int player_id, std::size_t card_number);
	void dealCards();
	void reset();
private:
	request_type createMessage(const json &);
	json createFinalBidMessage();

	int user_id_;
	int counter;
};

class SumScore : public Controller {
public:
	SumScore(Deck &, PlayersCollection &);
	~SumScore();
	stage sumUpScore();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	void resetPlayerAttributes(bool);
	bool isFinished() const;
private:
	request_type createMessage(stage stages_);
};

class Game : public Controller {
public:
	Game(Deck &, PlayersCollection &);
	~Game();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	const Card & playTurn(int, std::size_t);
	stage manageTurn(int, int);
	request_type createMessage(const stage stage_);
	int compareCardsAndPassToWinner();
	void setSuperiorSuit();
	void setStartingPlayer(int);
	void reset();
private:
	SumScore score_;
	std::vector<std::pair<int, Card> > vec_;
	int turn_counter_;
	int current_starting_player_;
	suits super_suit_;
	bool is_marriage_;
};



class GameManager {
public:
	GameManager();
	~GameManager();
	req doWork(std::size_t, const std::string &);
	void pushMessage(const request_type &);
protected:
	void removeIfLeaveCalled(const json& msg, int player_id);
	void returnExistingRooms(const json &);
	void attachClientIdsToMessage();
	int findGameId(size_t) const;
	void addPlayer(const json &);
	bool runGame(const json& msg);
	void createNewRoom(const json & msg);
	bool useEmptyRoom(const json & msg);

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
	std::vector<Controller*> employees_;
	GameManager & man_;
	size_t room_id_;
	Deck deck_;
	PlayersCollection players_;
	stage stage_;
};

#endif // _GAME_LOGIC_HPP_