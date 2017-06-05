#pragma once
#include <utility>
/**
 * @brief all figures present in Thousand game, and not_a_figure which is like a NULL value
 */
enum figures {
	NOT_A_FIGURE = -1,
	NINE = 0,
	TEN = 10,
	JACK = 2,
	QUEEN = 3,
	KING = 4,
	ACE = 11
};

/**
 * @brief all possible suits and NONE as a NULL value
 */
enum suits {
	NONE = 0,
	DIAMONDS = 80,
	CLUBS = 60,
	HEARTS = 100,
	SPADES = 40
};

/**
 * @brief class representing card
 */
class Card {
public:
	Card(figures, suits);
	Card(const Card& other);
	Card(Card&& other);
	Card operator=(const Card& card);
	Card operator=(Card&& card);
	bool operator==(const Card& other) const;
	bool operator!=(const Card& other) const;
	~Card();
	bool getIsUsed() const;
	void setIsUsed(bool is_used) const;
	const figures getFigure() const;
	const suits getSuit() const;
	const Card& isBigger(const Card& other, suits suit) const;
private:
	mutable bool is_used_; /// cards are not deleted from user decks, this variable defines whether they are still in use, or not
	suits suit_; /// card's parameter
	figures figure_; /// card's parameter
};