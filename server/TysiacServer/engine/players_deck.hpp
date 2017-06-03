#pragma once
#include <vector>
#include "card.hpp"

const int MIN_VALUE = 120;

class PlayersDeck {
public:
	PlayersDeck();
	~PlayersDeck();
	std::vector<Card>& getDeck();
	void clearDeck();
	void addCard(const Card& card);
	const Card& playCard(std::size_t card_number) const;
	bool doesHavePair(suits suit) const;
	const std::vector<int> getAllValidCards(const std::vector<Card>& vec, suits superior) const;
	std::size_t getMaxValue(bool isLateBid);
	bool isHigher(const Card& played, const Card& deck_card, suits superior) const;
private:
	bool findCard(figures figure, suits suit) const;
	std::vector<Card> deck_;
	std::size_t max_value_;
};