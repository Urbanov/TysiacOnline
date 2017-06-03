#pragma once
#include "controller.hpp"
#include "game_manager.hpp"
#include "leave_buster.hpp"
#include "chat_box.hpp"
#include "adder.hpp"
#include "bidder.hpp"
#include "dealer.hpp"
#include "game.hpp"

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