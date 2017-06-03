#pragma once
#include "controller.hpp"
class Starter : public Controller {
public:
	Starter(Deck &, PlayersCollection &);
	virtual ~Starter();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	json createStartMessage(const json & msg) const;
	bool getIsFull() const;
	void setIsFull(bool);
	virtual void reset();
private:
	bool isReadyToStart() const;
	void prepareToStart(stage);
	bool is_full_;
};