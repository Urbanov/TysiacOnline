#include "leave_buster.hpp"

LeaveBuster::LeaveBuster(Deck& deck, PlayersCollection& players, std::vector<PController>& controllers)
	: Controller(deck, players)
	, controllers_(controllers)
{}

LeaveBuster::~LeaveBuster()
{}

/**
 * @brief methods checks current room's stage, if game had started before player left, controller and model classes have reset() method called
 *
 * @param stage_ current game stage
 */
void LeaveBuster::resetIfGameHadStarted(const stage stage_)
{
	if (stage_ != ADDING) {
		for (auto& i : controllers_) {
			i->reset();
		}
	}
}
/**
 * @brief removes player from room and resets game
 *
 * @param msg leave type message sent by player
 * @param stage_ current game stage
 */
stage LeaveBuster::changeModel(const json& msg, const stage stage_)
{
	eraseLeaverFromRoom(msg["player"]);
	resetIfGameHadStarted(stage_);
	return newRoomStage(stage_);
}
/**
 * @brief finds and erases from room's player with given ID
 *
 * @param leaver_id ID of player to remove
 */
void LeaveBuster::eraseLeaverFromRoom(int leaver_id)
{
	for (auto i = players_.getArray().begin(); i != players_.getArray().end(); ++i) {
		if ((*i).getPlayerId() == leaver_id) {
			players_.getArray().erase(i);
			break;
		}
	}
}

/**
 * @brief returns new room's stage depending on given current stage
 *
 * @param stage_ current room's stage
 * @return new room's stage
 */
stage LeaveBuster::newRoomStage(const stage stage_)
{
	players_.resetPlayerAttributes(true);
	deck_.reset();
	if (stage_ != ADDING && stage_ != ENDING) {
		return LEAVING;
	}
	else return ADDING;
}

/**
 * @brief create messages for all players in room that inform them about player's leave
 *
 * @param msg leave type message sent by player
 * @param stage_ current game stage
 * @return messages informing other players who left
 */
request_type LeaveBuster::createMessages(const json& msg, const stage stage_)
{
	request_type request;
	json feedback = returnMessageHeader(msg, request);
	messageIfGameHadStarted(feedback, request, stage_);
	return request;
}

/**
 * @brief returns message leave type header
 * 
 * @param msg leave type message from player
 * @param request vector of messages sent to players
 * @return message containing header only
 */
json LeaveBuster::returnMessageHeader(const json& msg, request_type& request)
{
	json feedback;
	feedback["action"] = "leave";
	feedback["data"] = msg["player"];
	for (auto& i : players_.getArray()) {
		feedback["who"].push_back(i.getPlayerId());
	}
	request.push_back(feedback);
	return feedback;
}

/**
 * @brief creates additional messages if game had started
 * 
 * @param feedback message containing additional information
 * @param request vector of messages sent to players
 * @param stage_ current room's stage
 */
void LeaveBuster::messageIfGameHadStarted(json& feedback, request_type& request, const stage stage_)
{
	if (stage_ != ADDING && stage_ != ENDING) {
		feedback.erase("action");
		feedback.erase("data");
		feedback["player"] = -1;
		feedback["action"] = "end";
		request.push_back(feedback);
	}
}

