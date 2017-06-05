#pragma once
#include "controller.hpp"

/**
 * @brief part of controller, responsible for distributing cards and creating messages
 *
 */
class Dealer : public Controller {
public:
	Dealer(Deck& deck, PlayersCollection& players);
	virtual ~Dealer();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json& msg, const stage stage_);
	stage giveCardToPeer(int player_id, std::size_t card_number);
	void dealCards();
	virtual void reset();
private:
	request_type createMessage(const json& msg) const;
	json createFinalBidMessage() const;

	int user_id_; /// keeps id of player that was recently given a card
	int counter; /// counts how many cards have been given so far
	const int TWO_CARDS = 2; ///max number f cards a player can give to others
};