#include "starter.hpp"

Starter::Starter(Deck& deck, PlayersCollection& players)
	: Controller(deck, players)
	, is_full_(false)
{}

Starter::~Starter()
{}

stage Starter::changeModel(const json& msg, const stage stage_)
{
	if (stage_ == SUMMING_UP) {
		prepareToStart(stage_);
		return BIDDING;
	}
	players_.getPlayer(X, msg["player"]).setReady(true);
	if (isReadyToStart()) {
		prepareToStart(stage_);
		return BIDDING;
	}
	return ADDING;
}

request_type Starter::createMessages(const json& msg, const stage stage_)
{
	json feedback;
	request_type request;
	feedback["action"] = "ready";
	feedback["player"] = msg["player"];
	for (auto i : players_.getArray()) {
		if (i.getPlayerId() != msg["player"]) {
			feedback["who"].push_back(i.getPlayerId());
		}
	}
	request.push_back(feedback);
	return request;
}

json Starter::createStartMessage(const json& msg) const
{
	json feedback;
	feedback["action"] = "start";
	feedback["data"] = players_.getPlayer(HIGHEST).getScoreClass().getClaim();
	feedback["player"] = players_.getPlayer(HIGHEST).getPlayerId();
	for (auto& i : players_.getArray()) {
		feedback["who"].push_back(i.getPlayerId());
	}
	return feedback;
}

bool Starter::getIsFull() const
{
	return is_full_;
}

void Starter::setIsFull(bool is_full)
{
	is_full_ = is_full;
}

void Starter::reset()
{
	is_full_ = false;
}

bool Starter::isReadyToStart() const
{
	if (players_.getArray().size() != MAX_PLAYERS) {
		return false;
	}
	for (auto& i : players_.getArray()) {
		if (!i.getReady()) {
			return false;
		}
	}
	return true;
}

void Starter::prepareToStart(stage stage_)
{
	is_full_ = false;
	if (stage_ == SUMMING_UP) {
		players_.prepareGame(false);
	}
	else {
		players_.prepareGame(true);
	}
	deck_.shuffle();
	deck_.dealCards(players_.getArray());
}