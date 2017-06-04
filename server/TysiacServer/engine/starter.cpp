#include "starter.hpp"

Starter::Starter(Deck& deck, PlayersCollection& players)
	: Controller(deck, players)
	, is_full_(false)
{}

Starter::~Starter()
{}

/**
 * @brief part of controller, modifies model, handle all start type messages
 * @see Controller
 *
 * @param msg start type message received from player
 * @param stage_ current Room's stage
 *
 * @return new Room's stage
 */
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

/**
 * @brief creates ready-type update messages for all users in room 
 *
 * @param msg message received from player
 * @param stage_ current room's stage
 * 
 * @return vector of messages that will be sent to players
 */
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

/**
* @brief creates start-type update messages for all users in room
*
* @param msg message received from player
*
* @return start-type update message that will be sent to players
*/
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

/**
 * @brief checks if there are enough players to start a game
 * 
 * @return true if number of players is equal MAX_PLAYERS
 * @see MAX_PLAYERS
 */
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

/**
 * @brief prepares game to start
 *
 * @param stage_ current room's stage
 */
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