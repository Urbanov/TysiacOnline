#pragma once
#include "controller.hpp"


/**
 * @brief part of controller, class manages calculating and adding players' score, also creating score-type messages for players
 * @see Controller
 */
class SumScore : public Controller {
public:
	SumScore(Deck& deck, PlayersCollection& players, std::vector<PController>& controllers);
	virtual ~SumScore();
	stage sumUpScore();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json& msg, const stage stage_);
	virtual void reset();
	void resetPlayerAttributes(bool isFinal);
	bool isFinished() const;
private:
	json createScoreMessage() const;
	void createEndMessage(request_type& request, stage stage_);
	void resetAll();
	void createEndMessageBody(json& message, request_type& request);
	request_type createMessage(stage stages_);
	std::vector<PController>& controllers_;
	const int POINTS_WINNING_CAP = 1000; /// if any player gets this amount of points, game is over
};