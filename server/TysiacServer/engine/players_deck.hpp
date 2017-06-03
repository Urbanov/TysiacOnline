#pragma once
#include <vector>
#include "card.hpp"

const int MIN_VALUE = 120;

class PlayersDeck {
public:
	PlayersDeck();
	~PlayersDeck();
	std::vector<Card> & getDeck();
	void clearDeck();
	void addCard(const Card&);
	const Card & playCard(std::size_t) const;
	bool doesHavePair(suits);
	const std::vector<int> getAllValidCards(const std::vector<Card> &, suits) const;
	std::size_t getMaxValue(bool);
	bool isHigher(const Card &, const Card & deck_card, suits) const;
private:
	bool findCard(figures figure, suits suit) const;
	std::vector<Card> deck_;
	std::size_t max_value_;
};