#pragma once
#include "player.hpp"
#include <json.hpp>

using json = nlohmann::json;
using players = std::vector<Player>;
using request_type = std::vector<json>;

const int MAX_PLAYERS = 3; /// max number of players per room

enum iterators {
	X = -1, /// want to find player based of his ID
	CURRENT = 0, /// find current player (in terms of playing turn)
	COMPULSORY = 1, /// find player who has to bid 100 this round
	HIGHEST = 2, /// find player who bid the highest
};

/**
 * @brief class keeps all players' data and modifies it
 */
class PlayersCollection {
public:
	PlayersCollection();
	PlayersCollection(const PlayersCollection& other);
	~PlayersCollection();
	bool addPlayer(int player_id, std::string& nick);
	size_t getNextPlayer(iterators it);
	request_type getPlayerInfo() const;
	players& getArray();
	Player& getPlayer(iterators it, size_t player_id = 0);
	void setPlayer(iterators it, size_t player_id);
	void prepareGame(bool isFirst);
	void resetPlayerAttributes(bool isFinal);
private:
	players players_; /// vector of Player class objects
	std::vector<size_t> it_; /// vector of variables that are used to get proper player based on game's stage
};