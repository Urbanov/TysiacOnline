#pragma once
#include "controller.hpp"

class Adder : public Controller {
public:
	Adder(Deck &, PlayersCollection &);
	virtual ~Adder();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	bool isFull() const;

private:
	json acceptNewPlayer(const json & msg, stage stage_);
	request_type informOtherPlayers(const json & msg, stage stage_);
};