#include "adder.hpp"

Adder::Adder(Deck& deck, PlayersCollection& players)
	: Controller(deck, players)
{}

Adder::~Adder()
{}

/**
 * @brief adds new player to a room
 *
 * @param msg add request sent by a player
 * @param stage_ current server's stage
 * @return new server's stage
 */
stage Adder::changeModel(const json& msg, const stage stage_)
{
	std::string tmp = msg["data"];
	if (!players_.addPlayer(msg["player"], tmp)) {
		return FAIL;
	}
	return ADDING;
}

/**
 * @brief creates messages updating player's information
 about players in room
 *
 * @param msg add request sent to room by player
 * @param stage_ current server's stage
 * @return a vector of messages 
 */
request_type Adder::createMessages(const json& msg, const stage stage_)
{
	json feedback;
	request_type request;
	request = informOtherPlayers(msg, stage_);
	request.push_back(acceptNewPlayer(msg, stage_));
	return request;
}


bool Adder::isFull() const
{
	return (players_.getArray().size() == MAX_PLAYERS);
}

/**
 * @brief creates add request response
 *
 * @param msg add request sent to room by player
 * @param stage_ current server's stage
 * @return a response for player's add request
 */
json Adder::acceptNewPlayer(const json& msg, stage stage_) const
{
	json feedback;
	request_type request, player_info = players_.getPlayerInfo();
	feedback["who"] = msg["player"];
	feedback["action"] = "add";
	if (stage_ != FAIL) {
		feedback["error"] = false;
		for (auto& i : player_info) {
			feedback["data"].push_back(i);
		}
	}
	else {
		feedback["error"] = true;
	}
	return feedback;
}

/**
 * @brief creates message containing information about new player who joined the room
 *
 * @parm msg an add request
 * @param stage_ current server's stage
 * @return a vector of messages that will update client side
 */
request_type Adder::informOtherPlayers(const json& msg, stage stage_) const
{
	request_type request, player_info = players_.getPlayerInfo();
	json feedback;
	if (stage_ != FAIL) {
		if (players_.getArray().size() > 1) {
			feedback.clear();
			feedback["action"] = "new_player";
			for (auto& i : player_info) {
				if (i["id"] != msg["player"]) {
					feedback["who"].push_back(i["id"]);
				}
				else {
					feedback["data"] = i;
				}
			}
			request.push_back(feedback);
		}
	}
	return request;
}
