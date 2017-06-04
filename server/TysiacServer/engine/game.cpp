#include "game.hpp"

Game::Game(Deck& deck, PlayersCollection& players, std::vector<PController>& controllers)
	: score_(deck, players, controllers)
	, turn_counter_(0)
	, super_suit_(NONE)
	, Controller(deck, players)
	, is_marriage_(false)
{}

Game::~Game()
{}

/**
 * @brief calls manageTurn() that conducts the game. If manageTurn() returns SUMMING_UP, conducts end or a round
 *
 * @param msg player's message
 * @param stage_ current server's stage
 * @return new server's stage
 */
stage Game::changeModel(const json& msg, const stage stage_)
{
	if (manageTurn(msg["player"], msg["data"]) == SUMMING_UP) {
		return score_.sumUpScore();
	}
	return PLAYING;
}

/**
 * @brief creates messages that update player's view and call one of them to make a move
 *
 * @param msg player's message
 * @param stage_ current server's stage
 * @return new server's stage
 */
request_type Game::createMessages(const json& msg, stage stage_)
{
	request_type request = createMessage(stage_);
	if (stage_ == SUMMING_UP || stage_ == ADDING) {
		request_type tmp = score_.createMessages(msg, stage_);
		for (const auto& i : tmp) {
			request.push_back(i);
		}
	}
	return request;
}

/**
 * @brief plays on the table a card form player's deck
 */
const Card& Game::playTurn(int player, std::size_t card)
{
	return players_.getPlayer(X, player).getPlayersDeck().playCard(card);
}

/**
 * @brief plays a card from player's deck on the table, checking if it's a correct action before
 *
 * @param player player's id
 * @param card card's index in player's deck
 * @return new server's state, depending on model's current state
 */
stage Game::manageTurn(int player, int card)
{
	if (players_.getPlayer(CURRENT).getPlayerId() != player) {
		throw std::logic_error("Not player's turn to play a card");
	}
	playCardAndSetNexPlayer(player, card);
	if (allPlayersPlayedCard()) {
		return SUMMING_UP;
	}
	return PLAYING;
}

/**
 * @brief checks if there are three cards on the table and if all 8 turns have been played
 */
bool Game::allPlayersPlayedCard()
{
	if (vec_.size() == MAX_PLAYERS) {
		players_.setPlayer(CURRENT, compareCardsAndPassToWinner());
		if (++turn_counter_ == MAX_TURNS) {
			reset();
			return true;
		}
	}
	return false;
}

/**
 * @brief gets card from player's deck, places it on the table, sets trump if occurs and sets next player to play a card
 *
 * @param player player's id
 * @param card card's index in player's deck
 */
void Game::playCardAndSetNexPlayer(int player, int card)
{
	vec_.emplace_back(std::make_pair(player, playTurn(player, card)));
	setSuperiorSuit();
	players_.getNextPlayer(CURRENT);
}

/**
 * @brief creates detailed information about last card used by a player
 */
json Game::createPlayedCardDetails() const
{
	json tmpj = {
		{ "figure", vec_.back().second.getFigure() },
		{ "suit", vec_.back().second.getSuit() },
		{ "player", vec_.back().first },
		{ "marriage", is_marriage_ }
	};
	return tmpj;
}

/**
 * @brief createst hedaer for play-type message
 *
 * @param stage_ current room's stage
 */
json Game::createFeedbackHeader(const stage stage_) const
{
	json feedback = {
		{ "action", "play" },
		{ "player", players_.getPlayer(CURRENT).getPlayerId() }
	};
	if (stage_ == SUMMING_UP) {
		feedback.erase("player");
		feedback["player"] = -1;
	}
	return feedback;
}

/**
 * @brief checks if the turn is over and clears table if so
 */
void Game::ifEndOfTurn()
{
	if (vec_.size() == MAX_PLAYERS) {
		vec_.clear();
	}
}

/**
 * @brief get cards laying on the table
 */
std::vector<Card> Game::cardsOnTable()
{
	std::vector<Card> tmp;
	for (const auto& i : vec_) {
		tmp.push_back(i.second);
	}
	return tmp;
}

/**
 * @brief creates advanced message for player who will be playing a card next round
 *
 * @param player playing card
 * @param feedback message to the player
 * @request vector of messages to players
 */
void Game::createAdvancedMessage(Player& player, json& feedback, request_type& request)
{
	if (feedback["who"] == feedback["player"]) {
		feedback["data"]["available"] = player.getPlayersDeck().getAllValidCards(cardsOnTable(), super_suit_);
		request.push_back(feedback);
		feedback.erase("data");
	}
	else {
		request.push_back(feedback);
	}
}

/**
 * @brief adds addressee of the message and adds info about cards on the table
 *
 * @param feedback message already containing a header
 * @param request a vector of messages to players
 */
void Game::createUpdateMessagesToPlayers(json& feedback, request_type& request)
{
	json previous_cards = createPlayedCardDetails();
	ifEndOfTurn();
	for (auto& i : players_.getArray()) {
		feedback["who"] = i.getPlayerId();
		feedback["data"]["prev"] = previous_cards;
		createAdvancedMessage(i, feedback, request);
		feedback.erase("who");
	}
}

/**
 * @brief creates updating messages for players during gameplay
 *
 * @param stage_ current server's stage
 * @return a vector of messages that have been created
 */
request_type Game::createMessage(const stage stage_)
{
	request_type request;
	json feedback = createFeedbackHeader(stage_);
	createUpdateMessagesToPlayers(feedback, request);
	return request;
}
/**
 * @brief check if there is a chance of setting up a marriage
 */
bool Game::marriageMayOccur() const
{
	return vec_.size() == 1 && (vec_[0].second.getFigure() == KING || vec_[0].second.getFigure() == QUEEN);
}

/**
 * @brief check if first player in a turn has a king or a queen in his hand
 */
bool Game::playerHasKingQueenPair()
{
	return players_.getPlayer(X, vec_[0].first).getPlayersDeck().doesHavePair(
		vec_[0].second.getSuit());
}

/**
 * @brief modify model: add points for marriage to the claimer, and set new trumpsuit
 */
void Game::addMarriageAndSetTrump()
{
	super_suit_ = vec_[0].second.getSuit();
	players_.getPlayer(X, vec_[0].first).getScoreClass().addToTurnScore(super_suit_);
	is_marriage_ = true;
}
/**
 * @brief checks if a card creates a marriage and sets trumpsuit if so
 */
void Game::setSuperiorSuit()
{
	int score = 0;
	if (marriageMayOccur() && playerHasKingQueenPair()) {
		addMarriageAndSetTrump();
	}
	else {
		is_marriage_ = false;
	}
}

/**
 * @brief set winning card and find its owner
 */
void Game::chooseWinningCardAndSetWinningPlayer()
{
	Card winning_card = findSuperiorCard();
	for (const auto& i : vec_) {
		if (winning_card == i.second) {
			current_starting_player_ = i.first;
		}
	}
}

/**
 * @brief get value of three cards on table and add each value to player's score
 */
void Game::addScoreToPlayerForTakenCards()
{
	for (auto& i : vec_) {
		players_.getPlayer(X, current_starting_player_).getScoreClass().addToTurnScore(i.second.getFigure());
	}
}

/**
 * @brief compares cards on table and decides which player gets cards
 *
 * @return id of winning player
 */
int Game::compareCardsAndPassToWinner()
{
	chooseWinningCardAndSetWinningPlayer();
	addScoreToPlayerForTakenCards();
	return current_starting_player_;
}

/**
 * @brief find out which card is the highest
 */
Card Game::findSuperiorCard()
{
	Card first_winner = vec_[0].second.isBigger(vec_[1].second, super_suit_); 
	Card second_winner = vec_[0].second.isBigger(vec_[2].second, super_suit_);
	return first_winner.isBigger(second_winner, super_suit_);
}

/**
 * @brief sets player that is going to start next turn
 *
 * @param new_starting_player index of new starting player
 */
void Game::setStartingPlayer(int new_starting_player)
{
	current_starting_player_ = new_starting_player;
}

/**
 * @brief clears all variables' values
 */
void Game::reset()
{
	turn_counter_ = 0;
	current_starting_player_ = 0;
	super_suit_ = NONE;
}