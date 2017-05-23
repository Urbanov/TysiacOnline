#pragma once
#include "Controller.hpp"
#include "Starter.hpp"

const int MIN_RAISE = 10;

class Bidder : public Controller {
public:
	Bidder(Deck &, PlayersCollection &);
	virtual ~Bidder();

	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	stage bid(const json & msg, const stage stage_);
	virtual void reset();
private:
	request_type createUpdateInfo(const json & msg) const;
	request_type createSpecialInfo(const json & msg) const;
	json createStock(const json & msg) const;
	request_type createBid(const json & msg, stage stage_) const;
	request_type firstBid(stage stage_);
	request_type createCardDealingMessages() const;
	request_type createStarterMessages(const json & msg, stage stage_);

	mutable json additional_cards_;
	Starter starter_;
};