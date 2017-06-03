#pragma once
#include "controller.hpp"

const int TWO_CARDS = 2;

class Dealer : public Controller {
public:
	Dealer(Deck &, PlayersCollection &);
	virtual ~Dealer();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	stage giveCardToPeer(int player_id, std::size_t card_number);
	void dealCards();
	virtual void reset();
private:
	request_type createMessage(const json &);
	json createFinalBidMessage();

	int user_id_;
	int counter;
};