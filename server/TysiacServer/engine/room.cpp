#include "room.hpp"

Room::Room(int room_id, GameManager& man)
	: man_(man)
	, room_id_(room_id)
	, stage_(ADDING)
{
	employees_.emplace_back(std::make_unique<LeaveBuster>(deck_, players_, employees_));
	employees_.emplace_back(std::make_unique<ChatBox>(deck_, players_));
	employees_.emplace_back(std::make_unique<Adder>(deck_, players_));
	employees_.emplace_back(std::make_unique<Bidder>(deck_, players_));
	employees_.emplace_back(std::make_unique<Dealer>(deck_, players_));
	employees_.emplace_back(std::make_unique<Game>(deck_, players_, employees_));
}

Room::~Room()
{

}

void Room::changeStage(stage new_stage)
{
	stage_ = new_stage;
}

bool Room::runGame(const json& msg)
{
	request_type request, tmp;
	stage temp_stage;
	temp_stage = employees_[parse(msg["action"])]->changeModel(msg, stage_);
	request = employees_[parse(msg["action"])]->createMessages(msg, temp_stage);
	if (temp_stage == LEAVING) {
		stage_ = ADDING;
	}
	else {
		stage_ = temp_stage;
	}
	if (temp_stage == SUMMING_UP) {
		employees_[BID]->changeModel(msg, SUMMING_UP);
		tmp = employees_[BID]->createMessages(msg, SUMMING_UP);
		for (const auto& i : tmp) {
			request.push_back(i);
		}
		stage_ = BIDDING;
	}
	man_.pushMessage(request);
	if (temp_stage != FAIL) {
		return true;
	}
	return false;

}

int Room::parse(const std::string& str) const
{
	json command = { { "add", ADD },{ "deal", DEAL },{ "bid", BID },
	{ "play", PLAY },{ "chat", CHAT },{ "ready", BID },
	{ "leave", LEAVE },{ "disconnect", LEAVE }
	};
	return command[str];
}

bool Room::isEmpty()
{
	return players_.getArray().empty();
}

size_t Room::getRoomId() const
{
	return room_id_;
}

json Room::getPlayersInfo() const
{
	json players;
	players["id"] = room_id_;
	request_type info = players_.getPlayerInfo();
	for (auto& i : info) {
		players["nick"].push_back(i["nick"]);
	}
	return players;
}