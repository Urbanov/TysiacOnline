#include <stdexcept>
#include "players_deck.hpp"

PlayersDeck::PlayersDeck()
	: max_value_(0)
{}

PlayersDeck::~PlayersDeck() {}

std::vector<Card>& PlayersDeck::getDeck()
{
	return deck_;
}

void PlayersDeck::addCard(const Card& card)
{
	deck_.push_back(card);
	deck_.back().setIsUsed(false);
}

const Card & PlayersDeck::playCard(std::size_t card_number) const
{
	if (card_number >= deck_.size()) {
		throw std::out_of_range("No card with such index in player's deck");
	}
	if (deck_[card_number].getIsUsed()) {
		throw std::logic_error("Card has already been used, non existing");
	}
	deck_[card_number].setIsUsed(true);
	return deck_[card_number];
}

bool PlayersDeck::doesHavePair(suits suit) const
{
	return (findCard(KING, suit) || findCard(QUEEN, suit));
}

const std::vector<int> PlayersDeck::getAllValidCards(const std::vector<Card>& vec, suits superior) const
{
	std::vector<int> correct_cards, tmp;
	if (vec.empty()) {
		for (std::size_t i = 0; i < deck_.size(); ++i) {
			if (!deck_[i].getIsUsed()) {
				correct_cards.push_back(static_cast<int>(i));
			}
		}
		return correct_cards;
	}
	Card card = vec[0];
	if (vec.size() == 2 && vec[0].isBigger(vec[1], superior) == vec[1]) {
		card = vec[1];
	}
	for (std::size_t i = 0; i < deck_.size(); ++i) {
		if (isHigher(card, deck_[i], superior) && card.getSuit() == deck_[i].getSuit()) {
			correct_cards.push_back(static_cast<int>(i));
		}
	}
	if (correct_cards.empty()) {
		for (std::size_t i = 0; i < deck_.size(); ++i) {
			if ((vec[0].getSuit() == deck_[i].getSuit() && !deck_[i].getIsUsed())) {
				correct_cards.push_back(static_cast<int>(i));
			}
		}
	}
	if (correct_cards.empty()) {
		for (std::size_t i = 0; i < deck_.size(); ++i) {
			if (isHigher(card, deck_[i], superior)) {
				correct_cards.push_back(static_cast<int>(i));
			}
		}
	}
	if (correct_cards.empty()) {
		for (std::size_t i = 0; i < deck_.size(); ++i) {
			if (!deck_[i].getIsUsed()) {
				correct_cards.push_back(static_cast<int>(i));
			}
		}
	}
	return correct_cards;
}

std::size_t PlayersDeck::getMaxValue(bool isLateBid)
{
	if (max_value_ != 0 && !isLateBid) {
		return max_value_;
	}
	max_value_ = 0;
	max_value_ += MIN_VALUE;
	std::vector <std::pair<suits, figures> > vec = {
		{ DIAMONDS, NOT_A_FIGURE },
		{ CLUBS, NOT_A_FIGURE },
		{ HEARTS,NOT_A_FIGURE },
		{ SPADES, NOT_A_FIGURE }
	};
	for (auto i : deck_) {
		if (i.getFigure() == QUEEN || i.getFigure() == KING) {
			for (auto& j : vec) {
				if (j.first == i.getSuit()) {
					if (j.second != NOT_A_FIGURE) {
						max_value_ += j.first;
					}
					else {
						j.second = i.getFigure();
					}
				}
			}
		}
	}
	return max_value_;
}

bool PlayersDeck::findCard(figures figure, suits suit) const
{
	for (auto& i : deck_) {
		if (i.getFigure() == figure && i.getSuit() == suit && i.getIsUsed() == false) {
			return true;
		}
	}
	return false;
}

bool PlayersDeck::isHigher(const Card& played, const Card& deck_card, suits superior) const
{
	if (deck_card.getIsUsed()) {
		return false;
	}
	if (played.getSuit() == deck_card.getSuit()) {
		return (played.getFigure() < deck_card.getFigure());
	}
	if (played.getSuit() == superior) {
		return false;
	}
	if (deck_card.getSuit() == superior) {
		return true;
	}
	return false;
}

void PlayersDeck::clearDeck()
{
	deck_.clear();
	max_value_ = 0;
}
