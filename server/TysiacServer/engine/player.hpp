#pragma once
#include <string>
#include "score.hpp"
#include "players_deck.hpp"

/**
 * @brief class representing player, used during gameplay
 */
class Player {
public:
	Player(int player_id, std::string& nick);
	Player(const Player& other);
	Player& operator=(const Player& other);
	~Player();
	bool operator==(const Player& other) const;
	PlayersDeck& getPlayersDeck();
	size_t getPlayerId() const;
	const std::string& getPlayersNick() const;
	Score& getScoreClass();
	bool getReady() const;
	void setReady(bool ready);
private:
	bool ready_; ///< is player ready to start game when room is full
	Score score_; ///< player's score
	int player_id_; ///< player's unique ID
	std::string nick_; ///< player's nickname
	PlayersDeck players_deck_; ///< class containing player's cards
};