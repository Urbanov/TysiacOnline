#pragma once
#include <vector>
#include <random>
#include "Card.hpp"
#include "players_collection.hpp"

using players = std::vector<Player>;

const int MAX_CARDS = 7; /// Number of cards to deal to each player when a round starts


/** 
*	@brief class representing deck of cards which are dealt to players during game
*
*/
class Deck {
public:
	Deck(const std::vector<Card>&);
	Deck(const Deck& other);
	Deck();
	~Deck();
	void dealCards(players&);
	json addBonusCards(Player &);
	void shuffle();
	void reset();
private:
	std::random_device rd_; /// provides random shuffling of the deck
	std::vector<Card> deck_;
	std::vector<Card>::iterator deck_it_;
};