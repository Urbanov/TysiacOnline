#include "players_collection.hpp"

PlayersCollection::PlayersCollection()
{}

PlayersCollection::PlayersCollection(const PlayersCollection& other)
	: players_(other.players_)
{}

PlayersCollection::~PlayersCollection()
{}


players& PlayersCollection::getArray()
{
	return players_;
}
/**
 * @brief creates new player object with given ID and nick
 *
 * @return true if succeeded, false if there were too many players in room already @see MAX_PLAYERS
 */
bool PlayersCollection::addPlayer(int player_id, std::string& nick)
{
	if (players_.size() < MAX_PLAYERS) {
		players_.push_back(Player(player_id, nick));
		return true;
	}
	return false;
}

/** 
 * @brief looks for next player in player's collection
 *
 * @param it variable containing ID of a player based on which next player is found
 * @return ID of next player
 */
size_t PlayersCollection::getNextPlayer(iterators it)
{
	if (players_.size() != MAX_PLAYERS) {
		throw std::out_of_range("Game has not started");
	}
	if (it == X) {
		throw std::logic_error("Non existing iterator called in getNextPlayer() method");
	}
	for (size_t i = 0; i < players_.size(); ++i) {
		if (players_[i].getPlayerId() == it_[it]) {
			it_[it] = players_[(i + 1) % MAX_PLAYERS].getPlayerId();
			return it_[it];
		}
	}
	return -1;
}

/**
 * @brief return message containing information about IDs and nicks of players in colletion
 *
 * @return message with players' IDs and nicks
 */
request_type PlayersCollection::getPlayerInfo() const
{
	request_type vec;
	for (auto& i : players_) {
		json tmp;
		tmp["id"] = i.getPlayerId();
		tmp["nick"] = i.getPlayersNick();
		vec.push_back(tmp);
	}
	return vec;
}

/**
 * @brief gets player based on iterators it and id of player (it is considered in the first place)
 * 
 * @param it keeps ID of one of players, if X then method returns player based on player_id
 * @param player_id ID of player that will be returned
 * @return player object with given ID
 */
Player& PlayersCollection::getPlayer(iterators it, size_t player_id)
{
	if (players_.size() != MAX_PLAYERS) {
		throw std::out_of_range("Game has not started");
	}
	size_t index;
	if (it != X) {
		index = it_[it];
	}
	else {
		index = player_id;
	}
	for (auto& i : players_) {
		if (i.getPlayerId() == index)
			return i;
	}
	throw std::out_of_range("No player found based on given player id");
}

/**
 * @brief sets player_id as a current value of given it
 *
 * @param it index in vector it_ which keeps all players' IDs needed to conduct the game 
 * @param player_id ID to set in vec_
 */
void PlayersCollection::setPlayer(iterators it, size_t player_id)
{
	if (players_.size() != MAX_PLAYERS) {
		throw std::out_of_range("Game has not started");
	}
	if (it == X) {
		throw std::logic_error("Argument \"X\" passed to setPlayer");
	}
	it_[it] = player_id;
}

/**
 * @brief sets values of players' IDs in it_ vector, those will be needed to BIDDING and PLAYING stages
 *
 * @param isFirst is this the first round. It influences the algorithm setting up IDs.
 */
void PlayersCollection::prepareGame(bool isFirst)
{
	int i = isFirst ? 0 : (it_[COMPULSORY] + 1) % MAX_PLAYERS;
	it_.resize(MAX_PLAYERS);
	it_[CURRENT] = players_[(i + 1) % MAX_PLAYERS].getPlayerId();
	it_[COMPULSORY] = players_[i].getPlayerId();
	it_[HIGHEST] = players_[i].getPlayerId();
	players_[i].getScoreClass().setClaim(100, false);

}

/**
 * @brief resets all attributes, used for example when game is over or when someone leaves the game or between rounds
 *
 * @param isFinal if true, all the information is erased. Otherwise there are cleared only members that depend on each round
 */
void PlayersCollection::resetPlayerAttributes(bool isFinal)
{
	for (auto& i : players_) {
		i.getScoreClass().reset(isFinal);
		i.getPlayersDeck().clearDeck();
		if (isFinal) {
			i.setReady(false);
		}
	}
	if (isFinal) {
		it_.clear();
	}
}

