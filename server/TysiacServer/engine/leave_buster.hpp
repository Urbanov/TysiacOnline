#pragma once
#include "controller.hpp"

/**
 * @brief controller type class, handling all player leave events 
 * @see Controller
 */
class LeaveBuster : public Controller {
public:
	LeaveBuster(Deck& deck, PlayersCollection& players, std::vector<PController>& controllers);
	virtual ~LeaveBuster();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json& msg, const stage stage_);
private:
	json returnMessageHeader(const json & msg, request_type & request);
	void eraseLeaverFromRoom(int leaver_id);
	void messageIfGameHadStarted(json & feedback, request_type & request, const stage stage_);
	stage newRoomStage(const stage stage_);
	void resetIfGameHadStarted(const stage stage_);
	std::vector<PController>& controllers_;
};