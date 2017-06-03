#pragma once
#include <string>
#include "const_definitions.hpp"

class GameManager {
public:
	GameManager();
	~GameManager();
	req doWork(std::size_t, const std::string &);
	void pushMessage(const request_type &);
protected:
	void removeIfLeaveCalled(const json& msg, int player_id);
	void returnExistingRooms(const json &);
	void attachClientIdsToMessage();
	int findGameId(size_t) const;
	void addPlayer(const json &);
	bool runGame(const json& msg);
	void createNewRoom(const json & msg);
	bool useEmptyRoom(const json & msg);

	std::vector<std::vector<size_t> > players_;
	std::vector<PRoom> active_games_;
	request_type feedback_;
	req server_response_;
	std::size_t room_counter_;
};