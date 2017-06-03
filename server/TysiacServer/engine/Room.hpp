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
	Room(int room_id, GameManager& man);
	~Room();
	void changeStage(stage new_stage);
	bool runGame(const json& msg);
	int parse(const std::string& str) const;
	bool isEmpty();
	size_t getRoomId() const;
	json getPlayersInfo() const;
private:
	std::vector<PController> employees_;
	GameManager& man_;
	size_t room_id_;
	Deck deck_;
	PlayersCollection players_;
	stage stage_;
};