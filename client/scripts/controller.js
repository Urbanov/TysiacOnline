class Controller {
	constructor() {
		this.self = new SelfPlayer();
		this.view = new View();
		this.game = null;
		this.ws = null;
	}

	openConnection(address, port) {
		this.ws = new WebSocket("ws://" + address + ":" + port);

		this.ws.onopen = $.proxy(function (event) {
			this.requestRefresh();
		}, this);

		// handle messages from server
		this.ws.onmessage = $.proxy(function (event) {
			var msg = JSON.parse(event.data);

			switch (msg.action) {
				case "welcome":
					this.handleWelcome(msg);
					break;

				case "show":
					this.handleShow(msg);
					break;

				case "add":
					this.handleAdd(msg);
					break;

				case "new_player":
					this.handleNewPlayer(msg);
					break;

				case "chat":
					this.handleChat(msg);
					break;

				case "deal":
					this.handleDeal(msg);
					break;

				case "bid":
					this.handleBid(msg);
					break;

				case "stock":
					this.handleStock(msg);
					break;

				case "start":
					this.handleStart(msg);
					break;

				case "play":
					this.handlePlay(msg);
					break;

				case "score":
					this.handleScore(msg);
					break;

				case "end":
					this.handleEnd(msg);
					break;

				case "leave":
					this.handleLeave(msg);
					break;

				default:
					break;
			}
		}, this);
	}

	sendToServer(msg) {
		this.ws.send(JSON.stringify(msg));
	}

	login() {
		var nick = this.view.getNickname();
		if (!nick.length) {
			this.view.showLoginError();
		}
		else {
			this.view.hideModals();
			this.self.nick = nick;
		}
	}

	requestRefresh() {
		var msg = {
			action: "show"
		};
		this.sendToServer(msg);
	}

	requestRoom(event) {
		var msg = {
			action: "add",
			data: this.self.nick,
			id: event.data.id
		};
		this.sendToServer(msg);
	}

	loadRooms(data) {
		for (let room of data) {
			this.view.drawRoom(room);
		}
	}

	joinRoom(data) {
		this.game = new Game();
		this.view.changeScene();
		for (let player of data) {
			this.game.addPlayer(player);
		}
		this.self.index = this.game.players.length - 1;
	}

	leaveRoom() {
		this.game = null;
		var msg = {
			action: "leave"
		};
		this.sendToServer(msg);
		this.view.clearTable();
		this.view.clearChat();
		this.view.hideModals();
		this.view.changeScene();
		this.requestRefresh();
	}

	sendMessage() {
		var text = this.view.getChatMessage();
		if (text == "") {
			return;
		}
		var full_text = this.self.nick + ": " + text;

		this.view.addMessage(full_text);
		var msg = {
			action: "chat",
			data: full_text
		};
		this.sendToServer(msg);
	}

	sendBid(event) {
		this.view.hideModals();
		var msg = {
			action: "bid",
			data: event.data.value
		};
		this.sendToServer(msg);
	}

	sendReady() {
		this.view.hideModals();
		var msg = {
			action: "ready"
		};
		this.sendToServer(msg);
	}

	handleWelcome(msg) {
		this.self.id = msg.data;
	}

	handleShow(msg) {
		this.view.clearRoomList();
		if (msg.hasOwnProperty("data")) {
			this.loadRooms(msg.data);
		}
	}

	handleAdd(msg) {
		if (!msg.error) {
			this.view.hideJoinError();
			this.joinRoom(msg.data);
		}
		else {
			this.view.showJoinError();
			this.requestRefresh();
		}
	}

	handleNewPlayer(msg) {
		this.view.addMessage(msg.data.nick + " has entered the room");
		this.game.addPlayer(msg.data);
	}

	handleChat(msg) {
		this.view.addMessage(msg.data);
	}

	handleDeal(msg) {
		// check if game has just started
		if (this.game.round == 0) {
			this.game.startFirstRound();
		}

		// get cards from server and draw them
		for (let card of msg.data) {
			this.self.addCard(card);
		}
		this.view.drawCards();

		// change number of opponents' cards and draw them
		if (msg.data.length == 7) {
			for (let player of this.game.players) {
				player.n_cards = 7;
			}
			this.view.drawOpponentsCards();
		}
		else if (msg.data.length == 1) {
			for (let player of this.game.players) {
				if (player.bid == "pass") {
					++player.n_cards;
				}
				else {
					player.n_cards -= 2;
				}
			}
			this.view.drawOpponentsCards();
		}
	}

	handleBid(msg) {
		// first bid
		if (this.self.cards.length != 10 && msg.data.value == 100) {
			this.view.teaseStock();
		}

		// final bid
		else if (this.self.cards.length == 10) {
			for (let player of this.game.players) {
				if (player.id != this.self.id) {
					++player.n_cards;
				}
			}
			this.view.drawOpponentsCards();
		}

		if (msg.player != -1) {
			this.game.getPlayerById(msg.player).setTurn();
			this.view.drawTurns();
		}

		this.game.getPlayerById(msg.data.id).updateBid(msg.data.value);
		this.view.drawBids();
		if (msg.player == this.self.id) {
			this.view.showBids(msg.data.min, msg.data.max);
		}
	}

	handleStock(msg) {
		this.game.getPlayerById(msg.player).setTurn();
		this.view.drawTurns();
		this.game.getPlayerById(msg.player).n_cards = 10;
		this.view.drawOpponentsCards();

		this.view.displayStock(msg.data);
		if (msg.player == this.self.id) {
			for (let card of msg.data) {
				this.self.addCard(card);
			}
			this.view.drawCards();
			//$("#stock_modal").modal({ backdrop: false });
			this.view.showModal(Modals.STOCK);
			this.self.allAvailable();
		}
	}

	handleStart(msg) {
		this.view.clearBottom();
		for (let player of this.game.players) {
			if (player.id == msg.player) {
				player.updateBid(msg.data);
			}
			else {
				player.updateBid(0);
			}
		}
		this.view.drawBids();
		if (msg.player == this.self.id) {
			this.self.allAvailable();
		}
	}

	handlePlay(msg) {
		if (msg.player != -1) {
			this.game.getPlayerById(msg.player).setTurn();
			this.view.drawTurns();
		}
		this.game.rotate();

		if (msg.data.prev.marriage) {
			this.view.drawTrump(msg.data.prev.suit);
		}

		this.view.displayCard(msg.data.prev);
		if (this.self.id == msg.player) {
			this.self.someAvailable(msg.data.available);
		}
	}

	handleScore(msg) {
		for (let data of msg.data) {
			this.game.getPlayerById(data.player).updateScore(data.score);
		}
		this.view.drawScores();
		this.game.newRound();
	}

	handleEnd(msg) {
		for (let player of this.game.players) {
			player.resetScore();
		}
		this.game.round = 0;
		if (msg.player == -1) {
			var text = "jakis tchorz wyszedl i koniec gry :/";
		}
		else {
			var text = this.game.getPlayerById(msg.player).nick + " wins the game with " + msg.data + " points!";
		}
		this.view.drawEnd(text);
	}

	handleLeave(msg) {
		this.view.hideModals();
		this.view.addMessage(this.game.getPlayerById(msg.data).nick + " has left the room");
		for (let i in this.game.players) {
			if (this.game.players[i].id == msg.data) {
				this.game.players.splice(i, 1);
				break;
			}
		}
		this.self.clearCardData();
		this.self.index = (this.game.players[0].id == this.self.id ? 0 : 1);
	}
}