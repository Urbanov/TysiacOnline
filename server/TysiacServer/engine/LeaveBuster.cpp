#include "LeaveBuster.hpp"

LeaveBuster::LeaveBuster(Deck & deck, PlayersCollection & players, std::vector<PController>& controllers)
	: Controller(deck, players)
	, controllers_(controllers)
{}

LeaveBuster::~LeaveBuster()
{}

stage LeaveBuster::changeModel(const json & msg, const stage stage_)
{
	for (auto i = players_.getArray().begin(); i != players_.getArray().end(); ++i) {
		if ((*i).getPlayerId() == msg["player"]) {
			players_.getArray().erase(i);
			break;
		}
	}
	if (stage_ != ADDING) {
		for (auto& i : controllers_) {
			i->reset();
		}
	}

	players_.resetPlayerAttributes(true);
	deck_.reset();
	if (stage_ != ADDING && stage_ != ENDING) {
		return LEAVING;
	}
	else return ADDING;
}

request_type LeaveBuster::createMessages(const json & msg, const stage stage_)
{
	json feedback;
	request_type request;
	feedback["action"] = "leave";
	feedback["data"] = msg["player"];
	for (auto& i : players_.getArray()) {
		feedback["who"].push_back(i.getPlayerId());
	}
	request.push_back(feedback);
	if (stage_ != ADDING && stage_ != ENDING) {
		feedback.erase("action");
		feedback.erase("data");
		feedback["player"] = -1;
		feedback["action"] = "end";
		request.push_back(feedback);
	}
	return request;
}

