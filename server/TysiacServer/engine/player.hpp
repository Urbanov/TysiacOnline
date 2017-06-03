#pragma once
#include <string>
#include "score.hpp"
#include "players_deck.hpp"

class Player {
public:
	Player(int player_id, std::string& nick);
	Player(const Player& other);
	Player & operator=(const Player& other);
	~Player();
	bool operator==(const Player& other) const;
	PlayersDeck& getPlayersDeck();
	size_t getPlayerId() const;
	const std::string& getPlayersNick() const;
	Score& getScoreClass();
	bool getReady() const;
	void setReady(bool ready);
private:
	bool ready_;
	Score score_;
	int player_id_;
	std::string nick_;
	PlayersDeck players_deck_;
};