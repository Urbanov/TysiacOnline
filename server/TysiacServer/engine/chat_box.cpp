#include "chat_box.hpp"

ChatBox::ChatBox(Deck& deck, PlayersCollection& players)
	: Controller(deck, players)
{}

ChatBox::~ChatBox()
{}

stage ChatBox::changeModel(const json& msg, const stage stage_)
{
	return stage_;
}

/**
 * @brief creating message for every room member, containing message sent by one of them
 */
request_type ChatBox::createMessages(const json& msg, const stage stage_)
{
	request_type request;
	json response = {
		{ "action", "chat" },
		{ "player", msg["player"] },
		{ "data", msg["data"] },
	};
	for (auto& i : players_.getArray()) {
		if (i.getPlayerId() != msg["player"]) {
			response["who"].push_back(i.getPlayerId());
		}
	}
	request.push_back(response);
	return request;
}