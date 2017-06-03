#pragma once
#include <vector>
#include <random>
#include "Card.hpp"
#include "players_collection.hpp"

using players = std::vector<Player>;

const int MAX_CARDS = 7;

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
	std::random_device rd_;
	std::vector<Card> deck_;
	std::vector<Card>::iterator deck_it_;
};