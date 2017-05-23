#pragma once
#include <utility>

enum figures {
	NOT_A_FIGURE = -1,
	NINE = 0,
	TEN = 10,
	JACK = 2,
	QUEEN = 3,
	KING = 4,
	ACE = 11
};
enum suits {
	NONE = 0,
	DIAMONDS = 80,
	CLUBS = 60,
	HEARTS = 100,
	SPADES = 40
};

class Card {
public:
	Card(figures, suits);
	Card(const Card&);
	Card(Card&&);
	Card operator=(const Card&);
	Card operator=(Card&&);
	bool operator==(const Card& other) const;
	~Card();
	bool getIsUsed() const;
	void setIsUsed(bool) const;
	const figures getFigure() const;
	const suits getSuit() const;
	const Card & isBigger(const Card & other, suits suit) const;
private:
	mutable bool is_used_;
	figures figure_;
	suits suit_;
};