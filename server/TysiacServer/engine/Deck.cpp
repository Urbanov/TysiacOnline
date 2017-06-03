#include "deck.hpp"

Deck::Deck(const std::vector<Card>& deck)
	: deck_(deck)
	, deck_it_(deck_.begin())
{}

Deck::Deck(const Deck& other)
	: deck_(other.deck_)
	, deck_it_(deck_.begin())
{}

Deck::Deck()
{
	std::vector<figures> figure_ = { NINE, TEN, JACK, QUEEN, KING, ACE };
	std::vector<suits> suit_ = { DIAMONDS, CLUBS, HEARTS, SPADES };
	for (auto& i : figure_) {
		for (auto& j : suit_) {
			deck_.emplace_back(i, j);
		}
	}
	deck_it_ = deck_.begin();
}

Deck::~Deck() {}

/**
 * @brief deal out cards to players
 *
 * @param players collection of players who are to be given cards from Deck
 */
void Deck::dealCards(players& players)
{
	shuffle();
	for (auto& i : players) {
		i.getPlayersDeck().clearDeck();
		for (int j = 0; j < MAX_CARDS; ++j) {
			i.getPlayersDeck().addCard(*deck_it_++);
		}
	}
}

/**
 * @brief gives three cards to player who bade highest
 *
 * @param player is the one who bade highest
 * @return message containing given cards' details
 */
json Deck::addBonusCards(Player& player)
{
	json msg;
	while (deck_it_ != deck_.end()) {
		json tmp = {
			{ "figure", (*deck_it_).getFigure() },
			{ "suit", (*deck_it_).getSuit() }
		};
		msg.push_back(tmp);
		player.getPlayersDeck().addCard(*deck_it_++);
	}
	deck_it_ = deck_.begin();
	return msg;
}

/**
 * @brief does random shuffle on the deck
 */
void Deck::shuffle()
{
	std::mt19937 engine(rd_());
	std::shuffle(deck_.begin(), deck_.end(), engine);
}

/**
 * @brief resets position of iterator (used after giving additional cards)
 */
void Deck::reset()
{
	deck_it_ = deck_.begin();
}
