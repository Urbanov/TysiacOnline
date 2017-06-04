#include "game_manager.hpp"

GameManager::GameManager()
	: room_counter_(0)
{}

GameManager::~GameManager()
{}

/**
 * @brief attaches player's id to message sent by player, either tries to add player to a room or sends message to a room player belongs to
 * 
 * @param player_id id of message's author
 * @param message message sent by player before parsing
 * @return vector of messages and IDs of each addressee
 */
req GameManager::doWork(std::size_t player_id, const std::string& message)
{
	json parsedMessage = parseMessage(player_id, message);
	try {
		if (!runGame(parsedMessage)) {
			active_games_[findGameId(player_id)]->runGame(parsedMessage);
			removeIfLeaveCalled(parsedMessage, static_cast<int>(player_id));
			attachClientIdsToMessage();
		}
	}
	catch (const std::exception& e) {
		std::cout << "GameManager: " << e.what() << std::endl;
	}
	return server_response_;
}

/**
 * @brief given string message creates json message and attaches player's ID to it
 */
json GameManager::parseMessage(std::size_t player_id, const std::string& message)
{
	server_response_.clear();
	feedback_.clear();
	json msg = json::parse(message.begin(), message.end());
	msg["player"] = player_id;
	return msg;
}

/**
 * @brief removes player if the message type is "leave"
 *
 * @param msg player's message
 * @param player_id players's ID
 */
void GameManager::removeIfLeaveCalled(const json& msg, int player_id)
{
	if (msg["action"] != "leave") {
		return;
	}
	for (auto& i : players_) {
		for (auto it = i.begin(); it != i.end(); ++it) {
			if (*it == player_id) {
				i.erase(it);
				return;
			}
		}
	}
}

/**
 * @brief returns heading of message sent to player containing existing rooms detailed info 
 *
 * @param msg message from player
 */
json GameManager::createExistingRoomsHeading(const json& msg)
{
	json resp = {
		{ "action", "show" }
		,{ "who", msg["player"] }
	};
	return resp;
}

/**
 * @brief returns message sent to player containing existing rooms detailed info 
 *
 * @param msg message from player
 */
json GameManager::createExistingRoomsMessage(const json& msg)
{
	json resp = createExistingRoomsHeading(msg);
	for (auto& i : active_games_) {
		if (!i->isEmpty()) {
			resp["data"].push_back(i->getPlayersInfo());
		}
	}
	return resp;
}

/**
 * @brief creates message sent to player containing existing rooms detailed info
 */
void GameManager::returnExistingRooms(const json& msg)
{
	json resp = createExistingRoomsMessage(msg);
	feedback_.push_back(resp);
	attachClientIdsToMessage();
}

/**
 * @brief removes addrressee from messages and creates sepearate vector of IDs
 */
void GameManager::attachClientIdsToMessage()
{
	for (auto& i : feedback_) {
		std::vector<int> v;
		for (auto& a : i["who"]) {
			v.push_back(static_cast<int>(a));
		}
		i.erase("who");
		server_response_.emplace_back(std::make_pair(i.dump(), v));
	}
}

/**
 * @brief method used by rooms to pass messages to the manager
 *
 * @param vector of messages to be sent to players
 */
void GameManager::pushMessage(const request_type& msg)
{
	feedback_ = msg;
}

/**
 * @brief look for a game with given ID
 *
 * @return index of game in game's vector, or -1 if not found
 */
int GameManager::findGameId(size_t player_id) const
{
	for (size_t i = 0; i < players_.size(); ++i) {
		for (auto& j : players_[i]) {
			if (j == player_id) {
				return static_cast<int>(i);
			}
		}
	}
	return -1;
}

/**
 * @brief add played to the game or create new room depending on sent message
 *
 * @param msg message's message (ADD type)
 */
void GameManager::addPlayer(const json& msg)
{
	if (msg["id"] == -1) {
		if (!useEmptyRoom(msg)) {
			createNewRoom(msg);
		}
		return;
	}
	if (static_cast<size_t>(msg["id"]) < active_games_.size()) {
		if (active_games_[msg["id"]]->runGame(msg)) {
			players_[msg["id"]].push_back(msg["player"]);
		}
		attachClientIdsToMessage();
	}
}

/**
 * @brief checks if received message is one of the following types: ADD, SHOW, or DISCONNECT
 *
 * @param player's message
 * @return true if message is any of mentioned types, false otherwise
 */
bool GameManager::runGame(const json& msg)
{
	if (msg["action"] == "add") {
		addPlayer(msg);
		return true;
	}
	if (msg["action"] == "show") {
		returnExistingRooms(msg);
		return true;
	}
	if (msg["action"] == "disconnect" && findGameId(msg["player"]) == -1) {
		return true;
	}
	return false;
}

/**
 * @brief creates a new room and adds player who sent request to it
 *
 * @param msg create room request from player
 */
void GameManager::createNewRoom(const json& msg)
{
	active_games_.emplace_back(std::make_unique<Room>(room_counter_++, *this));
	if (active_games_.back()->runGame(msg)) {
		players_.emplace_back();
		players_.back().push_back(msg["player"]);
	}
	attachClientIdsToMessage();
}

/**
 * @brief look for empty room and add to it a player who requested for creating new room
 *
 * @param msg create room request
 * @return bool if such room is found and used
 */
bool GameManager::useEmptyRoom(const json& msg)
{
	for (size_t i = 0; i < active_games_.size(); ++i) {
		if (active_games_[i]->isEmpty()) {
			active_games_[i]->runGame(msg);
			players_[i].push_back(msg["player"]);
			attachClientIdsToMessage();
			return true;
		}
	}
	return false;
}