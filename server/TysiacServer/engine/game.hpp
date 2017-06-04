#pragma once
#include "controller.hpp"
#include "sum_score.hpp"

const int MAX_TURNS = 8;

class Game : public Controller {
public:
	Game(Deck& deck, PlayersCollection& players, std::vector<PController>& controllers);
	virtual ~Game();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json& msg, const stage stage_);
	const Card & playTurn(int player, std::size_t card);
	stage manageTurn(int player, int card);
	request_type createMessage(const stage stage_);
	int compareCardsAndPassToWinner();
	void setSuperiorSuit();
	void setStartingPlayer(int);
	virtual void reset();
private:
	Card findSuperiorCard();
	SumScore score_;
	std::vector<std::pair<int, Card>> vec_;
	int turn_counter_;
	int current_starting_player_;
	suits super_suit_;
	bool is_marriage_;
};