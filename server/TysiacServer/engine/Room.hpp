#pragma once
#include "Controller.hpp"
#include "GameManager.hpp"
#include "LeaveBuster.hpp"
#include "ChatBox.hpp"
#include "Adder.hpp"
#include "Bidder.hpp"
#include "Dealer.hpp"
#include "Game.hpp"

class GameManager;

class Room {
public:
	Room(int, GameManager&);
	~Room();
	void changeStage(stage);
	bool runGame(const json &);
	int parse(const std::string &);
	bool isEmpty();
	size_t getRoomId() const;
	json getPlayersInfo() const;
private:
	std::vector<PController> employees_;
	GameManager & man_;
	size_t room_id_;
	Deck deck_;
	PlayersCollection players_;
	stage stage_;
};