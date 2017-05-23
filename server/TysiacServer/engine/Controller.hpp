#pragma once
#include "Deck.hpp"
#include "PlayersCollection.hpp"

class Controller;

using PController = std::unique_ptr<Controller>;
using req = std::vector<std::pair<std::string, std::vector<int> > >;

enum commands {
	LEAVE = 0,
	DISCONNECT = 15,
	READY = 12,
	ADD = 2,
	DEAL = 4,
	BID = 3,
	PLAY = 5,
	CHAT = 1
};

enum stage {
	FAIL,
	LEAVING,
	ADDING,
	STARTING,
	BIDDING,
	DEALING,
	PLAYING,
	SUMMING_UP,
	ENDING
};

class Controller {
public:
	Controller(Deck & deck, PlayersCollection & players);
	virtual ~Controller();
	virtual stage changeModel(const json & msg, const stage stage_) = 0;
	virtual request_type createMessages(const json & msg, const stage stage_) = 0;
	virtual void reset();
protected:
	Deck & deck_;
	PlayersCollection & players_;
};