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