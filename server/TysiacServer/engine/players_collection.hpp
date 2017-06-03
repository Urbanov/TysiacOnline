#pragma once
#include "player.hpp"
#include <json.hpp>

using json = nlohmann::json;
using players = std::vector<Player>;
using request_type = std::vector<json>;

const int MAX_PLAYERS = 3;

enum iterators {
	X = -1,
	CURRENT = 0,
	COMPULSORY = 1,
	HIGHEST = 2,
};

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
	players players_;
	std::vector<size_t> it_;
};