#pragma once
#include "controller.hpp"


/**
 * @brief part of controller, adds players to the room
 */
class Adder : public Controller {
public:
	Adder(Deck& deck, PlayersCollection& players);
	virtual ~Adder();
	stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json& msg, const stage stage_);
	bool isFull() const;

private:
	json acceptNewPlayer(const json& msg, stage stage_) const;
	request_type informOtherPlayers(const json& msg, stage stage_) const;
};