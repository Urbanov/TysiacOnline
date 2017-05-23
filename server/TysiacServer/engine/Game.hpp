#pragma once
#include "Controller.hpp"
#include "SumScore.hpp"

const int MAX_TURNS = 8;

class Game : public Controller {
public:
	Game(Deck &, PlayersCollection &, std::vector<PController>& controllers);
	virtual ~Game();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json & msg, const stage stage_);
	const Card & playTurn(int, std::size_t);
	stage manageTurn(int, int);
	request_type createMessage(const stage stage_);
	int compareCardsAndPassToWinner();
	void setSuperiorSuit();
	void setStartingPlayer(int);
	virtual void reset();
private:
	SumScore score_;
	std::vector<std::pair<int, Card> > vec_;
	int turn_counter_;
	int current_starting_player_;
	suits super_suit_;
	bool is_marriage_;
};