#include "players_collection.hpp"

PlayersCollection::PlayersCollection()
{}

PlayersCollection::PlayersCollection(const PlayersCollection& other)
	: players_(other.players_)
{}

PlayersCollection::~PlayersCollection()
{}


players & PlayersCollection::getArray()
{
	return players_;
}

bool PlayersCollection::addPlayer(int player_id, std::string& nick)
{
	if (players_.size() < MAX_PLAYERS) {
		players_.push_back(Player(player_id, nick));
		return true;
	}
	return false;
}


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

Player & PlayersCollection::getPlayer(iterators it, size_t player_id)
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

void PlayersCollection::prepareGame(bool isFirst)
{
	int i = isFirst ? 0 : (it_[COMPULSORY] + 1) % MAX_PLAYERS;
	it_.resize(MAX_PLAYERS);
	it_[CURRENT] = players_[(i + 1) % MAX_PLAYERS].getPlayerId();
	it_[COMPULSORY] = players_[i].getPlayerId();
	it_[HIGHEST] = players_[i].getPlayerId();
	players_[i].getScoreClass().setClaim(100, false);

}

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

