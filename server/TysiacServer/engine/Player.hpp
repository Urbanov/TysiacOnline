#pragma once
#include <string>
#include "Score.hpp"
#include "PlayersDeck.hpp"

class Player {
public:
	Player(int, std::string &);
	Player(const Player& other);
	Player & operator=(const Player & other);
	~Player();
	bool operator==(const Player &) const;
	PlayersDeck & getPlayersDeck();
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
	PlayersDeck players_deck_;
};