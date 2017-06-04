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

/**
 * @biref Adds a card to player's deck and set is an unused
 *
 * @param card object added to player's deck
 */
void PlayersDeck::addCard(const Card& card)
{
	deck_.push_back(card);
	deck_.back().setIsUsed(false);
}

/**
 * @biref gets player's deck for card with given index
 *
 * @param card_number index of card in card vector deck_
 * @return Card with found at given index
 */
const Card& PlayersDeck::playCard(std::size_t card_number) const
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

/**
 * @brief check if player has unused queen or king of given suit
 */
bool PlayersDeck::doesHavePair(suits suit) const
{
	return (findCard(KING, suit) || findCard(QUEEN, suit));
}

/**
 * @brief checks if there are no cards on table, if so returns all unused cards' indices in player's deck
 * 
 * @param vec cards on table
 * @param correct_cards container for correct cards's indices 
 * @return true if there are no cards on the table
 */
bool PlayersDeck::onTableEmpty(const std::vector<Card>& vec, std::vector<int>& correct_cards) const
{
	if (vec.empty()) {
		for (std::size_t i = 0; i < deck_.size(); ++i) {
			if (!deck_[i].getIsUsed()) {
				correct_cards.push_back(static_cast<int>(i));
			}
		}
		return true;
	}
	return false;
}

/**
 * @brief returns highest card on the table
 *
 * @param vec contains cards on the table
 * @param superior current trumpsuit
 * @return highest card found in vec
 */
Card PlayersDeck::highestCardOnTheTable(const std::vector<Card>& vec, suits superior) const
{
	Card card = vec[0];
	if (vec.size() == 2 && vec[0].isBigger(vec[1], superior) == vec[1]) {
		card = vec[1];
	}
	return card;
}

/**
 * @brief find all cards the same suit as attacker's card and higher figure
 *
 * @param correct_cards container for correct cards' indices
 * @param card the highest card on the table
 * @param superior current trumpsuit
 * @return true if there was at least one card that qualified
 */
bool PlayersDeck::findSameSuitHigherFigure(std::vector<int>& correct_cards, const Card& card, suits superior) const
{
	bool return_value = false;
	for (std::size_t i = 0; i < deck_.size(); ++i) {
		if (isHigher(card, deck_[i], superior) && card.getSuit() == deck_[i].getSuit()) {
			correct_cards.push_back(static_cast<int>(i));
			return_value = true;
		}
	}
	return return_value;
}

/**
 * @brief find all cards the same suit as attacker's card
 *
 * @param correct_cards container for correct cards' indices
 * @param vec contains cards on the table
 * @return true if there was at least one card that qualified
 */
bool PlayersDeck::findSameSuit(std::vector<int>& correct_cards, const std::vector<Card>& vec) const
{
	bool return_value = false;
		for (std::size_t i = 0; i < deck_.size(); ++i) {
			if ((vec[0].getSuit() == deck_[i].getSuit() && !deck_[i].getIsUsed())) {
				correct_cards.push_back(static_cast<int>(i));
				return_value = true;
			}
		}
	return return_value;
}

/**
 * @brief finds all cards higher than highest on the table
 *
 * @param correct_cards container for correct cards' indices
 * @param superior current trumpsuit
 * @param card highest card on the table
 * @return true if there was at least one card that qualified
 */
bool PlayersDeck::findAnyHigherCard(std::vector<int>& correct_cards, suits superior, const Card& card) const
{
	bool return_value = false;
		for (std::size_t i = 0; i < deck_.size(); ++i) {
			if (isHigher(card, deck_[i], superior)) {
				correct_cards.push_back(static_cast<int>(i));
				return_value = true;
			}
		}
	return return_value;
}


/**
* @brief finds all unused cards
*
* @param correct_cards container for correct cards' indices
* @return true if there was at least one card that qualified
*/
bool PlayersDeck::findAnyCard(std::vector<int>& correct_cards) const
{
	bool return_value = false;
	if (correct_cards.empty()) {
		for (std::size_t i = 0; i < deck_.size(); ++i) {
			if (!deck_[i].getIsUsed()) {
				correct_cards.push_back(static_cast<int>(i));
				return_value = true;
			}
		}
	}
	return return_value;
}

/**
 * @brief get all cards that can be played given vec of cards already placed on the table
 *
 * @param vec cards on table based on which there are found cards that can be used
 * @param superior current trump suit
 * @return vector of indices of cards that can be used by player
 */
const std::vector<int> PlayersDeck::getAllValidCards(const std::vector<Card>& vec, suits superior) const
{
	std::vector<int> correct_cards, tmp;
	if (onTableEmpty(vec, correct_cards)) {
		return correct_cards;
	}
	Card card = highestCardOnTheTable(vec, superior);
	if (findSameSuitHigherFigure(correct_cards, card, superior) 
		|| findSameSuit(correct_cards, vec)  
		|| findAnyHigherCard(correct_cards, superior, card)   ) {
		return correct_cards;
	}
	findAnyCard(correct_cards);
	return correct_cards;
}

/**
 * @brief creates vector that will store info about existing marriages
 */
std::vector <std::pair<suits, figures>> PlayersDeck::getMarriageContainer() const
{
	std::vector <std::pair<suits, figures>> vec = {
		{ DIAMONDS, NOT_A_FIGURE },
		{ CLUBS, NOT_A_FIGURE },
		{ HEARTS,NOT_A_FIGURE },
		{ SPADES, NOT_A_FIGURE }
	};
	return vec;
}

/** 
 * @brief calculate the theoretical max value player can try to achieve with his cards
 * 
 * @param isLateBid checks if it is the last bid after the regular bidding
 * @return maximum value of player's cards
 */
std::size_t PlayersDeck::getMaxValue(bool isLateBid)
{
	if (max_value_ != 0 && !isLateBid) {
		return max_value_;
	}
	max_value_ = MIN_VALUE;
	addMarriageValue();
	return max_value_;
}

/**
 * @brief searches for queen or king in player's deck
 */
void PlayersDeck::addMarriageValue()
{
	std::vector <std::pair<suits, figures>> vec = getMarriageContainer();
	for (auto card : deck_) {
		if (card.getFigure() == QUEEN || card.getFigure() == KING) {
			lookForMarriage(vec, card);
		}
	}
}

/**
 * @brief either adds a figure to vec, or if one had been added, adds points to deck's max value
 *
 * @param vec contains info about found kings and queens
 * @param card object whose figure will be added to the vec
 */
void PlayersDeck::lookForMarriage(std::vector <std::pair<suits, figures>>& vec, Card& card)
{
	for (auto& j : vec) {
		if (j.first == card.getSuit()) {
			if (j.second != NOT_A_FIGURE) {
				max_value_ += j.first;
			}
			else {
				j.second = card.getFigure();
			}
		}
	}
}

/**
 * @brief check if player's deck contains a card with given figrure and suit
 *
 * @return true if a card was found, false otherwise
 */
bool PlayersDeck::findCard(figures figure, suits suit) const
{
	for (auto& i : deck_) {
		if (i.getFigure() == figure && i.getSuit() == suit && i.getIsUsed() == false) {
			return true;
		}
	}
	return false;
}

/**
 * @brief check if card deck_card is higher than played
 *
 * @param played one of the cards in comparison

 */
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

/**
 * @brief clears deck and resets max value
 */
void PlayersDeck::clearDeck()
{
	deck_.clear();
	max_value_ = 0;
}
