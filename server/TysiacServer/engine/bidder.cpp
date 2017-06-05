#include "bidder.hpp"

Bidder::Bidder(Deck& deck, PlayersCollection& players)
	: Controller(deck, players)
	, starter_(deck, players)
{}

Bidder::~Bidder()
{}

/**
 * @brief responsible for checking if player's bid is correct and executing it
 *
 * @param msg bid message from the most recent bidding player
 * @param stage_ current server's stage
 * @return new server's stage
 */
stage Bidder::bid(const json& msg, const stage stage_) const
{
	checkIfBidIsCorrect(msg);
	if (checkIfFinalBid(msg)) {
		return PLAYING;
	}
	makeBidAndSetHighest(msg);
	getNextPlayer();
	if (checkIfHighestIsCurrent()) {
		additional_cards_ = deck_.addBonusCards(players_.getPlayer(HIGHEST));
		return DEALING;
	}
	return BIDDING;
}

/**
 * @brief add to model bid given message contains, if it is not pass message set message's author as a highest bidder
 *
 * @param msg player's message
 */
void Bidder::makeBidAndSetHighest(const json& msg) const
{
	players_.getPlayer(CURRENT).getScoreClass().setClaim(msg["data"], false);
	if (msg["data"] != -1) {
		players_.setPlayer(HIGHEST, msg["player"]);
	}
}

/**
 * @brief checks if the player to claim is the one who has claimed the highest, needed to find specific situations
 */
bool Bidder::checkIfHighestIsCurrent() const
{
	return players_.getPlayer(CURRENT).getPlayerId() ==
		players_.getPlayer(HIGHEST).getPlayerId();
}

/**
 * @brief if player who bids is the one who has the highest bid, it means that it is the final bid done after regular bid
 *
 * @param msg message sent from player
 * @return true if it is the final bid
 */
bool Bidder::checkIfFinalBid(const json& msg) const
{
	if (checkIfHighestIsCurrent()) {
		players_.getPlayer(CURRENT).getScoreClass().setClaim(msg["data"], true);
		return true;
	}
	return false;
}

/**
 * @brief looks for a next player that did not resign from bidding
 */
void Bidder::getNextPlayer() const
{
	players_.getNextPlayer(CURRENT);
	while (players_.getPlayer(CURRENT).getScoreClass().getClaim() == -1) {
		players_.getNextPlayer(CURRENT);
	}
}

/**
 * @brief throws if command in player's message violates rules of game
 *
 * @param msg player's message
 */
void Bidder::checkIfBidIsCorrect(const json& msg) const
{
	//if message was sent by player not in his turn
	if (players_.getPlayer(CURRENT).getPlayerId() != msg["player"]) {
		throw std::logic_error("Player trying to bid not at his turn");
	}
	//if player is trying to claim less than he can
	if (players_.getPlayer(HIGHEST).getScoreClass().getClaim() > static_cast<int>(msg["data"])
		&& msg["data"] != -1) {
		throw std::logic_error("Player bids less than current highest bid");
	}
}

/**
 * @brief resets value of class members
 * used before each round
 */
void Bidder::reset()
{
	additional_cards_.clear();
	starter_.reset();
}

/**
 * @brief calls either starter, or bid method
 *
 * @param msg message from bidding player
 * @param stage_ current server's stage
 * @return stage return new server's stage
 */
stage Bidder::changeModel(const json& msg, const stage stage_)
{
	if (stage_ == ADDING || stage_ == SUMMING_UP) {
		return starter_.changeModel(msg, stage_);
	}
	return bid(msg, stage_);
}

/**
 * @brief creates message informing players about bid called by one of them
 *
 * @return vector of messages that will be broadcasted to all players in the room
 */
request_type Bidder::createMessages(const json& msg, stage stage_)
{
	request_type request, tmp;
	request = createStarterMessages(msg, stage_);
	if (request.empty()) {
		request = createBid(msg, stage_);
	}
	return request;
}

/**
 * @brief creates bid message that informs player that the bidding is over
 *
 * @param msg sent by most recent bidding player
 * @return vector of messages 
 */
request_type Bidder::createSpecialInfo(const json& msg) const
{
	request_type request;
	json feedback;
	feedback["action"] = "bid";
	for (auto i : players_.getArray()) {
		feedback["who"].push_back(i.getPlayerId());
	}
	feedback["player"] = -1;
	feedback["data"] = {
		{ "value", msg["data"] },
		{ "id", msg["player"] }
	};
	request.push_back(feedback);
	return request;
}

/**
 * @brief creates special message containing information about possible min and max value player can bid
 *
 * @param msg bid message sent by previous bidder
 * @return vector of messages to be sent to players
 */
request_type Bidder::createUpdateInfo(const json& msg) const
{
	json feedback = constructBidMessage(msg);
	request_type request;
	request.push_back(feedback);
	constructAdvancedBidMessage(feedback);
	request.push_back(feedback);
	return request;
}

/**
 * @brief construct bid message with min and max value next player can bid given standard message
 */
void Bidder::constructAdvancedBidMessage(json& feedback) const
{
	feedback.erase("who");
	feedback["who"] = players_.getPlayer(CURRENT).getPlayerId();
	feedback["data"]["min"] = players_.getPlayer(HIGHEST).getScoreClass().getClaim() + MIN_RAISE;
	feedback["data"]["max"] = players_.getPlayer(CURRENT).getPlayersDeck().getMaxValue(false);
}

/**
 * @brief constructing standard bid message
 */
json Bidder::constructBidMessage(const json& msg) const
{
	json feedback;
	for (auto i : players_.getArray()) {
		if (i.getPlayerId() != players_.getPlayer(CURRENT).getPlayerId()) {
			feedback["who"].push_back(i.getPlayerId());
		}
	}
	feedback["action"] = "bid";
	feedback["player"] = players_.getPlayer(CURRENT).getPlayerId();
	feedback["data"] = {
		{ "value", msg["data"] },
		{ "id", msg["player"] }
	};
	return feedback;
}

/**
 * @brief creates messages containing deteiled information about stock cards
 *
 * @param msg message sent by previous bidder
 * @return message including stock cards details
 */
json Bidder::createStock(const json& msg) const
{
	json feedback;
	feedback["action"] = "stock";
	feedback["player"] = players_.getPlayer(HIGHEST).getPlayerId();
	feedback["data"] = additional_cards_;
	for (const auto& i : players_.getArray()) {
		feedback["who"].push_back(i.getPlayerId());
	}
	additional_cards_.clear();
	return feedback;
}

/**
 * @brief creates different kind of messages depending on current server's stage
 *
 * @param msg message sent by previous bidder
 * @param stage_ current server's stage
 * @return vector of messages that will be sent to players
 */
request_type Bidder::createBid(const json& msg, stage stage_) const
{
	request_type request;
	if (stage_ == BIDDING || stage_ == SUMMING_UP) {
		request = createUpdateInfo(msg);
	}
	else if (stage_ == PLAYING) {
		request.push_back(starter_.createStartMessage(msg));
	}
	else if (stage_ == DEALING) {
		request = createSpecialInfo(msg);
		request.push_back(createStock(msg));
	}
	return request;
}

/**
 * @brief creates first bid with unique value - 100
 */
request_type Bidder::firstBid(stage stage_) const
{
	json feedback;
	request_type request;
	feedback["data"] = 100;
	feedback["player"] = players_.getPlayer(COMPULSORY).getPlayerId();
	request = createBid(feedback, stage_);
	return request;
}

/**
 * @brief creates information about received cards for every player
 *
 * @return vector of messages that will be sent to players
 */
request_type Bidder::createCardDealingMessages() const
{
	json feedback;
	request_type request;
	feedback["action"] = "deal";
	for (auto i : players_.getArray()) {
		feedback["who"] = i.getPlayerId();
		for (auto& j : i.getPlayersDeck().getDeck()) {
			json tmp = {
				{ "figure", j.getFigure() }
				,{ "suit", j.getSuit() }
			};
			feedback["data"].push_back(tmp);
		}
		request.push_back(feedback);
		feedback.erase("who");
		feedback.erase("data");
	}
	return request;
}

/**
 * @brief depending on current server's stage, creates messages for players 
 *
 * @param msg most recent message sent by any player
 * @param stage_ current server's stage
 * @return a vector of messages that will be sent to players
 */
request_type Bidder::createStarterMessages(const json& msg, stage stage_)
{
	request_type tmp, request;
	if (!starter_.getIsFull()) {
		if (stage_ == ADDING) {
			request = starter_.createMessages(msg, stage_);
		}
		if (stage_ == BIDDING || stage_ == SUMMING_UP) {
			tmp = createCardDealingMessages();
			for (const auto& i : tmp) {
				request.push_back(i);
			}
			tmp = firstBid(stage_);
			for (const auto& i : tmp) {
				request.push_back(i);
			}
			starter_.setIsFull(true);
		}
	}
	return request;
}