#pragma once
#include "controller.hpp"
#include "starter.hpp"


/**
 * @brief class manages bidding and gives cards to a player who bade the highest
 */
class Bidder : public Controller {
public:
	Bidder(Deck& deck, PlayersCollection& players);
	virtual ~Bidder();

	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json& msg, const stage stage_);
	stage bid(const json& msg, const stage stage_) const;
	virtual void reset();
private:
	void makeBidAndSetHighest(const json& msg) const;
	bool checkIfHighestIsCurrent() const;
	bool checkIfFinalBid(const json& msg) const;
	void getNextPlayer() const;
	void checkIfBidIsCorrect(const json& msg) const;
	request_type createUpdateInfo(const json& msg) const;
	void constructAdvancedBidMessage(json& feedback) const;
	json constructBidMessage(const json& msg) const;
	request_type createSpecialInfo(const json& msg) const;
	json createStock(const json& msg) const;
	request_type createBid(const json& msg, stage stage_) const;
	request_type firstBid(stage stage_) const;
	request_type createCardDealingMessages() const;
	request_type createStarterMessages(const json& msg, stage stage_);

	mutable json additional_cards_; ///< cards which are given to bidding winner
	Starter starter_;  ///< Manages ready-type messages and tarts the game
	const int MIN_RAISE = 10; ///<minimum raise of claim 
};