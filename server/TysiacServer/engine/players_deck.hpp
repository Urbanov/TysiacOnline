#pragma once
#include <vector>
#include "card.hpp"

const int MIN_VALUE = 120; /// value of full deck

/**
 * @brief class representing player's cards, calculating cards's value and finding available cards given cards on the table
 */
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
	void addMarriageValue();
	void lookForMarriage(std::vector<std::pair<suits, figures>>& vec, Card& card);
	std::vector<std::pair<suits, figures>> getMarriageContainer() const;
	bool findSameSuit(std::vector<int>& correct_cards, const std::vector<Card>& vec) const;
	bool findAnyHigherCard(std::vector<int>& correct_cards, suits superior, const Card& card) const;
	bool findAnyCard(std::vector<int>& correct_cards) const;
	bool onTableEmpty(const std::vector<Card>& vec, std::vector<int>& correct_cards) const;
	Card highestCardOnTheTable(const std::vector<Card>& vec, suits superior) const;
	bool findSameSuitHigherFigure(std::vector<int>& correct_cards, const Card& card, suits superior) const;
	bool findCard(figures figure, suits suit) const;
	
	std::vector<Card> deck_;
	std::size_t max_value_;
};