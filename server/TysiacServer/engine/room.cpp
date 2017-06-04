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

/**
 * @brief parses message's headling, then sends message to proper controller class and sends created messages back
 * Returns created messages using method on GameManager
 *
 * @param msg message received from player
 * @return true if player was successfully added to the room (return value used for adding feedback only)
 */
bool Room::runGame(const json& msg)
{
	request_type request, tmp;
	stage temp_stage;
	temp_stage = employees_[parse(msg["action"])]->changeModel(msg, stage_);
	request = employees_[parse(msg["action"])]->createMessages(msg, temp_stage);
	setNewRoomStage(temp_stage);
	stageEqualsSummingUp(msg, tmp, request, temp_stage);
	return pushServerMessages(temp_stage, request);
}

/**
 * @brief pushes created messages to manager who sends it to players, returns true if a player was successfully added to the game
 */
bool Room::pushServerMessages(stage temp_stage, request_type& request)
{
	man_.pushMessage(request);
	if (temp_stage != FAIL) {
		return true;
	}
	return false;
}

/**
 * @brief sets new room stage
 */
void Room::setNewRoomStage(stage temp_stage)
{
	if (temp_stage == LEAVING) {
		stage_ = ADDING;
	}
	else {
		stage_ = temp_stage;
	}
}

/**
 * @brief if stage equals SUMMING_UP deals cards again
 */
void Room::stageEqualsSummingUp(const json& msg, request_type& tmp, request_type& request, stage temp_stage)
{
	if (temp_stage == SUMMING_UP) {
		employees_[BID]->changeModel(msg, SUMMING_UP);
		tmp = employees_[BID]->createMessages(msg, SUMMING_UP);
		for (const auto& i : tmp) {
			request.push_back(i);
		}
		stage_ = BIDDING;
	}
}

/**
 * @brief choose correct enum value based on message's heading
 *
 * @param heading message's heading containing type of message
 * @return enum representing type of message that room can make use of
 */
int Room::parse(const std::string& heading) const
{
	json command = { { "add", ADD },{ "deal", DEAL },{ "bid", BID },
	{ "play", PLAY },{ "chat", CHAT },{ "ready", BID },
	{ "leave", LEAVE },{ "disconnect", LEAVE }
	};
	return command[heading];
}

/**
 * @brief check if there is any player in the room
 */
bool Room::isEmpty()
{
	return players_.getArray().empty();
}

size_t Room::getRoomId() const
{
	return room_id_;
}

/**
 * @brief returns json message containing room's id and nicks of players inside
 *
 */
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