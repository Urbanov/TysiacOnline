class Game {
	constructor() {
		this.players = [];
		this.timeout;
		this.counter = 0;
		this.round = 0;
	}

	getPlayerById(id) {
		for (let player of this.players) {
			if (player.id == id) {
				return player;
			}
		}
	}

	addPlayer(player) {
		if (player.id == controller.self.id) {
			this.players.push(controller.self);
		}
		else {
			this.players.push(new Player(player.id, player.nick));
		}
		this.checkReady();
	}

	checkReady() {
		// room is full
		if (this.players.length == 3) {
			controller.view.askReady();
		}
	}

	newRotation() {
		this.counter = 0;
		this.timeout = setTimeout(function () {
			controller.view.clearTop();
			controller.view.clearBottom();
		}, 2000);
	}

	startFirstRound() {
		for (let player of this.players) {
			player.resetScore();
		}
		controller.view.drawPlayers();
		controller.view.drawScores();
		this.newRound();
	}

	rotate() {
		++this.counter;
		if (this.counter == 1) {
			clearTimeout(this.timeout);
			controller.view.clearBottom();
			controller.view.clearTop();
		}
		else if (this.counter == 3) {
			this.newRotation();
		}
	}

	newRound() {
		controller.self.clearCardData();
		for (let player of this.players) {
			player.updateBid(0);
		}
		controller.view.drawBids();
		++this.round;
	}
}