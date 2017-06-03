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

stage Game::changeModel(const json& msg, const stage stage_)
{
	if (manageTurn(msg["player"], msg["data"]) == SUMMING_UP) {
		return score_.sumUpScore();
	}
	return PLAYING;
}

request_type Game::createMessages(const json& msg, stage stage_)
{
	request_type request = createMessage(stage_);
	if (stage_ == SUMMING_UP || stage_ == ADDING) {
		request_type tmp = score_.createMessages(msg, stage_);
		for (const auto & i : tmp) {
			request.push_back(i);
		}
	}
	return request;
}

const Card & Game::playTurn(int player, std::size_t card)
{
	return players_.getPlayer(X, player).getPlayersDeck().playCard(card);
}

stage Game::manageTurn(int player, int card)
{
	if (players_.getPlayer(CURRENT).getPlayerId() != player) {
		throw std::logic_error("Not player's turn to play a card");
	}
	vec_.emplace_back(std::make_pair(player, playTurn(player, card)));
	setSuperiorSuit();
	players_.getNextPlayer(CURRENT);
	if (vec_.size() == MAX_PLAYERS) {
		players_.setPlayer(CURRENT, compareCardsAndPassToWinner());
		if (++turn_counter_ == MAX_TURNS) {
			reset();
			return SUMMING_UP;
		}
	}
	return PLAYING;
}

request_type Game::createMessage(const stage stage_)
{
	request_type request;
	json feedback = {
		{ "action", "play" },
		{ "player", players_.getPlayer(CURRENT).getPlayerId() }
	};
	if (stage_ == SUMMING_UP) {
		feedback.erase("player");
		feedback["player"] = -1;
	}
	json tmpj = {
		{ "figure", vec_.back().second.getFigure() },
		{ "suit", vec_.back().second.getSuit() },
		{ "player", vec_.back().first },
		{ "marriage", is_marriage_ }
	};
	if (vec_.size() == MAX_PLAYERS) {
		vec_.clear();
	}
	std::vector<Card> tmp;
	for (const auto& i : vec_) {
		tmp.push_back(i.second);
	}
	for (auto& i : players_.getArray()) {
		feedback["who"] = i.getPlayerId();
		feedback["data"]["prev"] = tmpj;
		if (feedback["who"] == feedback["player"]) {
			feedback["data"]["available"] = i.getPlayersDeck().getAllValidCards(tmp, super_suit_);
			request.push_back(feedback);
			feedback.erase("data");
		}
		else {
			request.push_back(feedback);
		}
		feedback.erase("who");
	}
	return request;
}

void Game::setSuperiorSuit()
{
	int score = 0;
	if (vec_.size() == 1 && (vec_[0].second.getFigure() == KING || vec_[0].second.getFigure() == QUEEN)) {
		if (players_.getPlayer(X, vec_[0].first).getPlayersDeck().doesHavePair(
			vec_[0].second.getSuit())) {
			super_suit_ = vec_[0].second.getSuit();
			players_.getPlayer(X, vec_[0].first).getScoreClass().addToTurnScore(super_suit_);
			is_marriage_ = true;
		}
	}
	else {
		is_marriage_ = false;
	}
}

int Game::compareCardsAndPassToWinner()
{
	Card winning_card = findSuperiorCard();
	for (const auto& i : vec_) {
		if (winning_card == i.second) {
			current_starting_player_ = i.first;
		}
	}
	for (auto& i : vec_) {
		players_.getPlayer(X, current_starting_player_).getScoreClass().addToTurnScore(i.second.getFigure());
	}
	return current_starting_player_;
}

Card Game::findSuperiorCard()
{
	Card first_winner = vec_[0].second.isBigger(vec_[1].second, super_suit_); 
	Card second_winner = vec_[0].second.isBigger(vec_[2].second, super_suit_);
	return first_winner.isBigger(second_winner, super_suit_);
}

void Game::setStartingPlayer(int new_starting_player)
{
	current_starting_player_ = new_starting_player;
}

void Game::reset()
{
	turn_counter_ = 0;
	current_starting_player_ = 0;
	super_suit_ = NONE;
}