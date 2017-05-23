#include "GameManager.hpp"

GameManager::GameManager()
	: room_counter_(0)
{}

GameManager::~GameManager()
{}

req GameManager::doWork(std::size_t player_id, const std::string & message)
{
	server_response_.clear();
	feedback_.clear();
	json msg = json::parse(message.begin(), message.end());
	msg["player"] = player_id;
	try {
		if (!runGame(msg)) {
			active_games_[findGameId(player_id)]->runGame(msg);
			removeIfLeaveCalled(msg, player_id);
			attachClientIdsToMessage();
		}
	}
	catch (const std::exception& e) {
		std::cout << "GameManager: " << e.what() << std::endl;
	}
	return server_response_;
}

void GameManager::removeIfLeaveCalled(const json& msg, int player_id)
{
	if (msg["action"] != "leave") {
		return;
	}
	for (auto & i : players_) {
		for (auto it = i.begin(); it != i.end(); ++it) {
			if (*it == player_id) {
				i.erase(it);
				return;
			}
		}
	}
}

void GameManager::returnExistingRooms(const json & msg)
{
	json resp = {
		{ "action", "show" }
		,{ "who", msg["player"] }
	};
	for (auto& i : active_games_) {
		if (!i->isEmpty()) {
			resp["data"].push_back(i->getPlayersInfo());
		}
	}
	feedback_.push_back(resp);
	attachClientIdsToMessage();
}

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

void GameManager::pushMessage(const request_type & msg)
{
	feedback_ = msg;
}

int GameManager::findGameId(size_t player_id) const
{
	for (size_t i = 0; i < players_.size(); ++i) {
		for (auto& j : players_[i]) {
			if (j == player_id) {
				return i;
			}
		}
	}
	return -1;
}

void GameManager::addPlayer(const json & msg)
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

bool GameManager::runGame(const json & msg)
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

void GameManager::createNewRoom(const json & msg)
{
	active_games_.emplace_back(std::make_unique<Room>(room_counter_++, *this));
	if (active_games_.back()->runGame(msg)) {
		players_.emplace_back();
		players_.back().push_back(msg["player"]);
	}
	attachClientIdsToMessage();
}

bool GameManager::useEmptyRoom(const json & msg)
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