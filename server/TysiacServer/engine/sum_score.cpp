#include "sum_score.hpp"

SumScore::SumScore(Deck& deck, PlayersCollection& players, std::vector<PController>& controllers)
	: Controller(deck, players)
	, controllers_(controllers)
{}

SumScore::~SumScore()
{}

/**
 * @brief rounds up temporary score of each player, then adds it to their overall score
 *
 * @return new Room's stage
 */
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

/**
 * @brief calls sumUpScore() method, used to tkae advantage on virtual method mechanism
 *
 * @return new Room's stage
 */
stage SumScore::changeModel(const json& msg, const stage stage_)
{
	return sumUpScore();
}

/**
* @brief calls createMessage method, used to tkae advantage on virtual method mechanism
*
* @return vector of messages that will be sent to players
*/
request_type SumScore::createMessages(const json& msg, stage stage_)
{
	return createMessage(stage_);
}

void SumScore::reset()
{
	resetPlayerAttributes(true);
}

/**
* @brief clears players' game data
*
* @param isFinal set to true if for example game is finished or someone leaves room
*/
void SumScore::resetPlayerAttributes(bool isFinal)
{
	players_.resetPlayerAttributes(isFinal);
	if (isFinal) {
		return;
	}
	players_.prepareGame(false);
}

/**
* @brief checks if someone's score reached PINTS_WINNING_CAP
*
* @return true if someone reaches POINTS_WINNING_CAP
*/
bool SumScore::isFinished() const
{
	for (auto& i : players_.getArray()) {
		if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
			return true;
		}
	}
	return false;
}


/**
 * @brief creates score-type update message for each player
 *
 * @return message containing information about score of each player
 */
json SumScore::createScoreMessage() const
{
	json message;
	message["action"] = "score";
	for (auto& i : players_.getArray()) {
		message["who"].push_back(i.getPlayerId());
		json tmp = {
			{ "player", i.getPlayerId() },
			{ "score", i.getScoreClass().getTurnScore() }
		};
		message["data"].push_back(tmp);
	}
	return message;
}
	
/**
 * @brief create end-type message for all players
 *
 * @param request message container
 * @param stage_ current room's stage
 */
void SumScore::createEndMessage(request_type& request, stage stage_)
{
	if (stage_ == ADDING) {
		json message = {
			{ "action", "end" }
		};
		createEndMessageBody(message, request);
		resetAll();
	}
}

/**
 * @brief reset all controller and model classes
 */
void SumScore::resetAll()
{
	for (auto& i : controllers_) {
		i->reset();
	}
}

/**
 * @brief creates boddy for end-type message containing ID and points of winner
 *
 * @param end-type message heading that needs to be filled with data
 * @param request message container
 */
void SumScore::createEndMessageBody(json& message, request_type& request)
{
	for (auto& i : players_.getArray()) {
		if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
			message["data"] = i.getScoreClass().getScore();
			message["player"] = i.getPlayerId();
		}
		message["who"].push_back(i.getPlayerId());
	}
	request.push_back(message);
}

/**
 * @brief creates score-type message, if stage is ADDING creates also end-type message
 *
 * @param stage_ current room's stage
 * @return vector of messages which will be sent to players
 */
request_type SumScore::createMessage(stage stage_)
{
	request_type request;
	request.push_back(createScoreMessage());
	reset();
	createEndMessage(request, stage_);
	return request;
}