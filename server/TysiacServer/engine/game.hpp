#pragma once
#include "controller.hpp"
#include "sum_score.hpp"


/**
 * @brief part of controller, class handling gameplay. Handles all play-type messages from clients. Modifies model and creates update messages for players
 * @see Controller
 */
class Game : public Controller {
public:
	Game(Deck& deck, PlayersCollection& players, std::vector<PController>& controllers);
	virtual ~Game();
	virtual stage changeModel(const json& msg, const stage stage_);
	virtual request_type createMessages(const json& msg, const stage stage_);
	const Card& playTurn(int player, std::size_t card);
	stage manageTurn(int player, int card);
	request_type createMessage(const stage stage_);
	int compareCardsAndPassToWinner();
	void setSuperiorSuit();
	void setStartingPlayer(int);
	virtual void reset();
private:
	void chooseWinningCardAndSetWinningPlayer();
	void addScoreToPlayerForTakenCards();
	bool marriageMayOccur() const;
	bool playerHasKingQueenPair();
	void addMarriageAndSetTrump();
	void playCardAndSetNexPlayer(int player, int card);
	json createPlayedCardDetails() const;
	json createFeedbackHeader(const stage stage_) const;
	void ifEndOfTurn();
	std::vector<Card> cardsOnTable();
	void createAdvancedMessage(Player& player, json& feedback, request_type& request);
	void createUpdateMessagesToPlayers(json& feedback, request_type& request);
	bool allPlayersPlayedCard();

	Card findSuperiorCard();
	SumScore score_; ///< representing player's score and claim
	std::vector<std::pair<int, Card>> vec_; ///< used when calculating the winner of a turn
	int turn_counter_; ///< counts up to 8, then round is over
	int current_starting_player_; ///< ID of current player to start a turn
	suits super_suit_; ///< representing current trumpsuit
	bool is_marriage_; ///< set to true if there is marriage, used only to produce game-type update messages for clients
	const int MAX_TURNS = 8; ///< number of turns each round
};