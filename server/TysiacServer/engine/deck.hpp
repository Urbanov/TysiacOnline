#pragma once
#include <vector>
#include <random>
#include "card.hpp"
#include "players_collection.hpp"

using players = std::vector<Player>;



/** 
 * @brief class representing deck of cards which are dealt to players during game
 *
 */
class Deck {
public:
	Deck(const std::vector<Card>& deck);
	Deck(const Deck& other);
	Deck();
	~Deck();
	void dealCards(players& players);
	json addBonusCards(Player& player);
	void shuffle();
	void reset();
private:
	std::random_device rd_; /// provides random shuffling of the deck
	std::vector<Card> deck_; /// vector containing all cards in a deck
	std::vector<Card>::iterator deck_it_; /// used to deal cards and then give additional cards to a player
	const int MAX_CARDS = 7; /// number of cards to deal to each player when a round starts
};