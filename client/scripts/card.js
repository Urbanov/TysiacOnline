var Suits = {
	NONE: 0,
	SPADES: 40,
	CLUBS: 60,
	DIAMONDS: 80,
	HEARTS: 100
};

class Card {
	constructor(figure, suit) {
		this.figure = figure;
		this.suit = suit;
		this.used = false;
		this.available = false;
	}

	static order(suit) {
		switch (suit) {
			case Suits.HEARTS:
				return 1;
				break;

			case Suits.DIAMONDS:
				return 3;
				break;

			case Suits.CLUBS:
				return 2;
				break;

			case Suits.SPADES:
				return 4;
				break;
		}
	}
}