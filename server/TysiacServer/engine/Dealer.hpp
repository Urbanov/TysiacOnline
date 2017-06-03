#pragma once
#include "controller.hpp"

const int TWO_CARDS = 2; ///max number f cards a player can give to others
/**
*@brief part of controller, responsible for distributing cards and creating messages
*
*/
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

	int user_id_;/// keeps id of player that was recently given a card
	int counter; /// counts how many cards have been given so far
};