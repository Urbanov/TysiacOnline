#pragma once
#include <string>
#include "const_definitions.hpp"

/**
 * @brief class receiving messages from clients and passing them to proper rooms, also managing all room objects @see Room
 */
class GameManager {
public:
	GameManager();
	~GameManager();
	req doWork(std::size_t player_id, const std::string& message);
	void pushMessage(const request_type& msg);

private:
	json parseMessage(std::size_t player_id, const std::string & message);
	void removeIfLeaveCalled(const json& msg, int player_id);
	json createExistingRoomsHeading(const json & msg);
	json createExistingRoomsMessage(const json & msg);
	void returnExistingRooms(const json& msg);
	void attachClientIdsToMessage();
	int findGameId(size_t player_id) const;
	void addPlayer(const json& msg);
	bool runGame(const json& msg);
	void createNewRoom(const json& msg);
	bool useEmptyRoom(const json& msg);

	std::vector<std::vector<size_t>> players_;
	std::vector<PRoom> active_games_;
	request_type feedback_;
	req server_response_;
	std::size_t room_counter_;
};