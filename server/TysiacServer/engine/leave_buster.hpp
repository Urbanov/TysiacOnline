#pragma once
#include "controller.hpp"

class LeaveBuster : public Controller {
public:
	LeaveBuster(Deck& deck, PlayersCollection& players, std::vector<PController>& controllers);
	virtual ~LeaveBuster();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json& msg, const stage stage_);
private:
	std::vector<PController>& controllers_;
};