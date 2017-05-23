#pragma once
#include "Controller.hpp"

class ChatBox : public Controller {
public:
	ChatBox(Deck&, PlayersCollection&);
	virtual ~ChatBox();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
};