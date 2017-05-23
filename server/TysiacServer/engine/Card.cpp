#include "Card.hpp"


Card::Card(figures fig, suits s)
	: figure_(fig)
	, suit_(s)
	, is_used_(false)
{}

Card::Card(const Card & other)
	: figure_(other.figure_)
	, suit_(other.suit_)
	, is_used_(other.is_used_)
{}

Card::Card(Card && other)
	: figure_(std::move(other.figure_))
	, suit_(std::move(other.suit_))
	, is_used_(std::move(other.is_used_))
{}

Card Card::operator=(const Card & card)
{
	this->figure_ = card.figure_;
	this->suit_ = card.suit_;
	this->is_used_ = card.is_used_;
	return *this;
}

Card Card::operator=(Card && card)
{
	this->figure_ = card.figure_;
	this->suit_ = card.suit_;
	this->is_used_ = card.is_used_;
	return *this;
}

bool Card::operator==(const Card & other) const
{
	return (suit_ == other.suit_ && figure_ == other.figure_);
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

const Card & Card::isBigger(const Card & other, suits suit) const
{
	if (suit_ == other.suit_) {
		if (figure_ > other.figure_) {
			return *this;
		}
		else {
			return other;
		}
	}
	else if (other.suit_ == suit) {
		return other;
	}
	return *this;
}

