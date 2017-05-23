#include "Adder.hpp"

Adder::Adder(Deck & deck, PlayersCollection & players)
	: Controller(deck, players)
{}

Adder::~Adder()
{}

stage Adder::changeModel(const json & msg, const stage stage_)
{
	std::string tmp = msg["data"];
	if (!players_.addPlayer(msg["player"], tmp)) {
		return FAIL;
	}
	return ADDING;
}

request_type Adder::createMessages(const json & msg, const stage stage_)
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

json Adder::acceptNewPlayer(const json & msg, stage stage_)
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

request_type Adder::informOtherPlayers(const json & msg, stage stage_)
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