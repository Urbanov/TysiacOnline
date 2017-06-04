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

/**
 * @brief class conducting a single game
 */
class Room {
public:
	Room(int room_id, GameManager& man);
	~Room();
	void changeStage(stage new_stage);
	bool runGame(const json& msg);
	bool pushServerMessages(stage temp_stage, request_type& request);
	int parse(const std::string& str) const;
	bool isEmpty();
	size_t getRoomId() const;
	json getPlayersInfo() const;
private:
	void setNewRoomStage(stage temp_stage);
	void stageEqualsSummingUp(const json& msg, request_type& tmp, request_type& request, stage temp_stage);
	std::vector<PController> employees_; ///vector of all game controllers
	GameManager& man_; /// manager that keeps all games and passes messages to them
	size_t room_id_; ///unique id based on which game is identified
	Deck deck_; ///deck used in this game
	PlayersCollection players_; ///collection of players that enter the room
	stage stage_; /// current game's stage
};