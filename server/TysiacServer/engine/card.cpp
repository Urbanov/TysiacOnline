#include "card.hpp"


Card::Card(figures fig, suits s)
	: figure_(fig)
	, suit_(s)
	, is_used_(false)
{}

Card::Card(const Card& other)
	: figure_(other.figure_)
	, suit_(other.suit_)
	, is_used_(other.is_used_)
{}

Card::Card(Card&& other)
	: figure_(std::move(other.figure_))
	, suit_(std::move(other.suit_))
	, is_used_(std::move(other.is_used_))
{}

Card Card::operator=(const Card& card)
{
	this->figure_ = card.figure_;
	this->suit_ = card.suit_;
	this->is_used_ = card.is_used_;
	return *this;
}

Card Card::operator=(Card&& card)
{
	this->figure_ = card.figure_;
	this->suit_ = card.suit_;
	this->is_used_ = card.is_used_;
	return *this;
}

bool Card::operator==(const Card& other) const
{
	return (suit_ == other.suit_ && figure_ == other.figure_);
}

bool Card::operator!=(const Card& other) const
{
	return suit_ != other.suit_ || figure_ != other.figure_;
}

Card::~Card() {}

bool Card::getIsUsed() const
{
	return is_used_;
}

void Card::setIsUsed(bool is_used) const
{
	is_used_ = is_used;
}

const figures Card::getFigure() const
{
	return figure_;
}

const suits Card::getSuit() const
{
	return suit_;
}

/**
 * @brief checks which card is higher, where the card which calls the method has slight advantage as "attacking" card
 *
 * @param other other card to compare with
 * @param trump current trumpsuit
 * @return higher card
 */
const Card& Card::isBigger(const Card& other, suits trump) const
{
	//If both cards are the same suit, a card with higher figure is higher
	if (suit_ == other.suit_) {
		return figure_ > other.figure_ ? *this : other;
	}
	//if suit doesn't match, check if the second card is trump
	else if (other.suit_ == trump) {
		return other;
	}
	//if it was not trump, this card has to be higher
	return *this;
}
