class Player {
	constructor(id, nick) {
		this.id = id;
		this.nick = nick;
		this.score = 0;
		this.bid = 0;
		this.n_cards = 0;
		this.turn = false;
	}

	resetScore() {
		this.score = 0
	}

	updateScore(score) {
		this.score += score;
	}

	updateBid(value) {
		this.bid = value != -1 ? value : "pass";
	}

	setTurn() {
		for (let player of controller.game.players) {
			if (player.id == this.id) {
				player.turn = true;
			}
			else {
				player.turn = false;
			}
		}
	}
}

class SelfPlayer extends Player {
	constructor(id, nick) {
		super(id, nick);
		this.index = undefined;
		this.cards = [];
		this.used = 0;
	}

	getLeftPlayer() {
		return controller.game.players[(this.index + 1) % 3];
	}

	getRightPlayer() {
		return controller.game.players[(this.index + 2) % 3];
	}

	addCard(card) {
		this.cards.push(new Card(card.figure, card.suit));
	}

	useCard(event) {
		var id = Number(event.data.value);

		var test = ">>> USECARD: " + event.data.value;
		console.log(test);

		if (!this.cards[id].available) {
			console.log(">>> USECARD: card is not available");
			return;
		}

		this.cards[id].used = true;
		this.cards[id].available = false;
		++this.used;
		--this.n_cards;
		controller.view.removeCardImg(id);

		// give away cards after getting stock
		if (this.cards.length == 10 && this.used == 2) {
			this.giveAwayCards(id);
			return;
		}

		// play card
		if (this.cards.length == 8 || this.used > 2) {
			this.playCard(id);
		}
	}

	giveAwayCards(second_card) {
		this.noneAvailable();
		for (var first_card in this.cards) {
			if (this.cards[first_card].used && first_card != second_card) {
				break;
			}
		}
		let msg = {
			action: "deal",
			data: [{
				player: this.getLeftPlayer().id, // left player
				card: Number(first_card)
			}, {
				player: this.getRightPlayer().id, // right player
				card: Number(second_card)
			}]
		};
		console.log(">>> give cards: " + JSON.stringify(msg));
		controller.sendToServer(msg);
	}

	playCard(id) {
		this.noneAvailable();
		var msg = {
			action: "play",
			data: id
		};
		console.log(">>> send play: " + JSON.stringify(msg));
		controller.sendToServer(msg);
	}

	noneAvailable() {
		for (let card of this.cards) {
			card.available = false;
		}
	}

	allAvailable() {
		for (let card of this.cards) {
			card.available = true;
		}
	}

	someAvailable(array) {
		this.noneAvailable();
		for (let id of array) {
			this.cards[id].available = true;
		}
	}

	clearCardData() {
		this.cards = [];
		this.used = 0;
	}
}