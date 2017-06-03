#include "sum_score.hpp"

SumScore::SumScore(Deck& deck, PlayersCollection& players, std::vector<PController>& controllers)
	: Controller(deck, players)
	, controllers_(controllers)
{}

SumScore::~SumScore()
{}

stage SumScore::sumUpScore()
{
	for (auto& i : players_.getArray()) {
		i.getScoreClass().roundScore();
		i.getScoreClass().addScore(i.getScoreClass().getTurnScore());
		if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
			return ADDING;
		}
	}
	return SUMMING_UP;
}

stage SumScore::changeModel(const json& msg, const stage stage_)
{
	return sumUpScore();
}

request_type SumScore::createMessages(const json& msg, stage stage_)
{
	return createMessage(stage_);
}

void SumScore::reset()
{
	resetPlayerAttributes(true);
}

void SumScore::resetPlayerAttributes(bool isFinal)
{
	players_.resetPlayerAttributes(isFinal);
	if (isFinal) {
		return;
	}
	players_.prepareGame(false);
}

bool SumScore::isFinished() const
{
	for (auto& i : players_.getArray()) {
		if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
			return true;
		}
	}
	return false;
}

request_type SumScore::createMessage(stage stages_)
{
	json message;
	request_type request;
	message["action"] = "score";
	for (auto& i : players_.getArray()) {
		message["who"].push_back(i.getPlayerId());
		json tmp = {
			{ "player", i.getPlayerId() },
			{ "score", i.getScoreClass().getTurnScore() }
		};
		message["data"].push_back(tmp);
	}
	request.push_back(message);
	resetPlayerAttributes(false);
	if (stages_ == ADDING) {
		json message = {
			{ "action", "end" }
		};
		for (auto& i : players_.getArray()) {
			if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
				message["data"] = i.getScoreClass().getScore();
				message["player"] = i.getPlayerId();
			}
			message["who"].push_back(i.getPlayerId());
		}
		request.push_back(message);
		for (auto& i : controllers_) {
			i->reset();
		}
		reset();
	}
	return request;
}