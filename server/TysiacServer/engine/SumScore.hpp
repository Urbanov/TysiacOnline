#pragma once
#include "Controller.hpp"

const int POINTS_WINNING_CAP = 1000;

class SumScore : public Controller {
public:
	SumScore(Deck &, PlayersCollection &, std::vector<PController>& controllers);
	virtual ~SumScore();
	stage sumUpScore();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	virtual void reset();
	void resetPlayerAttributes(bool);
	bool isFinished() const;
private:
	request_type createMessage(stage stages_);
	std::vector<PController>& controllers_;
};