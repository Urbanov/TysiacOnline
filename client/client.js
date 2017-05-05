var Figures = {
	NINE: 0,
	TEN: 10,
	JACK: 2,
	QUEEN: 3,
	KING: 4,
	ACE: 11
};

var Suits = {
	SPADES: 40,
	CLUBS: 60,
	DIAMONDS: 80,
	HEARTS: 100
}

class Card {
	constructor(figure, suit) {
		this.figure = figure;
		this.suit = suit;
		this.used = false;
		this.available = false;
	}
}

class Identity {
	constructor(id, nick) {
		this.id = id;
		this.nick = nick;
	}
}

class Player extends Identity {
	constructor(id, nick) {
		super(id, nick);
		this.cards = [];
		this.used = 0;
		this.index = undefined;
	}
}

class PlayerGameInfo extends Identity {
	constructor(id, nick) {
		super(id, nick);
		this.score = 0;
		this.bid = 0;
		this.ready = false;
		this.cards = 0;
	}
}

// global variables
var ws;
var self;
var game = [];

//FIXME
var counter = 0;

$(document).ready(function () {

	// create player object
	self = new Player();
	self.nick = "cenzotest"; //FIXME xD

	console.log(self);

	// initial event listeners
	$("#create_room").click({ id: -1 }, requestRoom);
	$("#refresh_list").click(requestRefresh);
	$("#send").click(sendMessage);
	$("#text_area").keypress(function (event) {
		if (event.keyCode == 13) {
			event.preventDefault();
			sendMessage();
		}
	});
	$("#leave").click(leaveRoom);
	//$("#login_modal").modal({ backdrop: "static" }); //FIXME
	$("#login_modal").on("shown.bs.modal", function () {
		$("#nickname").focus();
	});
	$("#login").click(login);
	$("#ready").click(sendReady);

	// open a connection
	ws = new WebSocket("ws://192.168.43.124:2137");

	// connection opened, refresh server list
	ws.onopen = function (event) {
		requestRefresh();
	}

	// handle messages from server
	ws.onmessage = function (event) {
		var msg = JSON.parse(event.data);

		///////////////////////
		console.log(">>> RECEIVED: " + JSON.stringify(msg));

		switch (msg.action) {
			case "welcome":
				self.id = msg.data;
				break;

			case "show":
				if (msg.hasOwnProperty("data")) {
					loadRooms(msg.data);
				}
				break;

			case "add":
				if (!msg.error) {
					$("#join_error").hide();
					joinRoom(msg.data);
				}
				else {
					$("#join_error").show();
					requestRefresh();	
				}
				break;

			case "new_player":
				addMessage(msg.data.nick + " has entered the room");
				addPlayer(msg.data);
				break;

			case "chat":
				addMessage(msg.data);
				break;

			case "deal":
				for (let card of msg.data) {
					addCard(card);
					//drawCard(self.cards.length - 1);
				}
				drawCards();
				break;

			case "bid":
				updateBid(msg.data.id, msg.data.value);
				if (msg.player == self.id) {
					showBids(msg.data.min, msg.data.max);
				}
				break;

			case "stock":
				displayStock(msg.data);
				if (msg.player == self.id) {
					for (let card of msg.data) {
						addCard(card);
						//drawCard(self.cards.length - 1);
					}
					drawCards();
					handleStock();
				}
				break;

			case "start":
				clearBottom();
				for (let player of game) {
					if (player.id == msg.player) {
						updateBid(player.id, msg.data);
					}
					else {
						updateBid(player.id, 0);
					}
				}
				if (msg.player == self.id) {
					console.log(">>> jedziesz");
					allAvailable();
				}
				break;

			case "play":
				displayCard(msg.data.prev);
				if (self.id == msg.player) {
					console.log(">>> jedziesz");
					someAvailable(msg.data.available);
				}
				// display card
				//console.log(">>> DISPLAY: " + JSON.stringify(msg.data.prev));
				break;

			case "score":
				self.cards = [];
				break;

			default:
				// ready is unused for now
				//console.log(">>> UNUSED: " + JSON.stringify(msg));
				break;
		}
	}
});

function login() {
	var nick = $("#nickname").val();
	if (!nick.length) {
		$("#login_error").show();
	}
	else {
		$("#login_modal").modal("hide");
		self.nick = nick;
	}
}

function leaveRoom() {
	alert("xD");
	$("#lobby").show();
	$("#game_panel").hide();
}

function sendMessage() {
	var text = self.nick + ":\xa0" + $("#text_area").val();
	$("#text_area").val("");
	addMessage(text);
	var msg = {
		action: "chat",
		data: text
	}
	sendToServer(msg);
}

function addMessage(msg) {
	$('#chatbox').append($("<div>", {
		text: msg
	}));
}

function requestRefresh() {
	var msg = {
		action: "show"
	};
	sendToServer(msg);
}

function requestRoom(event) {
	var msg = {
		action: "add",
		data: self.nick,
		id: event.data.id
	};
	sendToServer(msg);
}

function joinRoom(data) {
	$("#lobby").hide();
	$("#game_panel").show();
	for (let player of data) {
		addPlayer(player);
	}
	self.index = game.length - 1;
}

function addPlayer(player) {
	game.push(new PlayerGameInfo(player.id, player.nick));
	
	// room is full
	if (game.length == 3) {
		askReady();
	}
}

function showBids(min, max) {
	$("#bids_modal").modal({ backdrop: false });
	$(".modal-backdrop").appendTo(".game");

	// normal bid (includes pass button)
	if (self.cards.length != 10) {
		var bid = 110;
		var elem = $("<button/>", {
			text: "pass",
			class: "btn btn-default btn-danger"
		});
		elem.click({ value: -1 }, sendBid);
		$("#bids").append(elem);
	}

	// final bid after getting stock (can keep 100)
	else {
		var bid = 100;
	}

	// show other buttons
	for (; bid < min; bid += 10) {
		var elem = $("<button/>", {
			text: bid,
			class: "btn btn-default bid_button disabled"
		});
		$("#bids").append(elem);
	}

	for (; bid <= max; bid += 10) {
		var elem = $("<button/>", {
			text: bid,
			class: "btn btn-default bid_button"
		});
		elem.click({ value: bid }, sendBid);
		$("#bids").append(elem);
	}

	for (; bid <= 360; bid += 10) {
		var elem = $("<button/>", {
			text: bid,
			class: "btn btn-default bid_button disabled"
		});
		$("#bids").append(elem);
	}
}

function sendBid(event) {
	$("#bids").html("");
	$('#bids_modal').modal("hide");
	var msg = {
		action: "bid",
		data: event.data.value
	}
	sendToServer(msg);
}

function loadRooms(data) {
	$("#room_list").html("");
	for (let room of data) {
		var players = room.nick.join(", ");
		var elem = $("<button/>", {
			text: players,
			class: "room btn btn-block btn-default"
		});
		if (room.nick.length < 3) {
			elem.click({ id: room.id }, requestRoom);
		}
		else {
			elem.prop("disabled", true);
		}
		$("#room_list").append(elem);
	}
}

function askReady() {
	$("#ready_modal").modal({ backdrop: false });
	$(".modal-backdrop").appendTo(".game");
}

function sendReady() {
	$('#ready_modal').modal("hide");
	var msg = {
		action: "ready"
	};
	sendToServer(msg);
}

function addCard(card) {
	self.cards.push(new Card(card.figure, card.suit));
}

function updateBid(player, value) {
	for (let i in game) {
		if (game[i].id == player) {
			game[i].bid = value != -1 ? value : "pass";
			break;
		}
	}
}

function displayStock(cards) {
	$("#bottom_left").prop("src", path(cards[0]));
	$("#bottom_middle").prop("src", path(cards[1]));
	$("#bottom_right").prop("src", path(cards[2]));
}

function clearBottom() {
	$("#bottom_left").prop("src", "images/empty.png");
	$("#bottom_middle").prop("src", "images/empty.png");
	$("#bottom_right").prop("src", "images/empty.png");
}

function clearTop() {
	$("#top_left").prop("src", "images/empty.png");
	$("#top_right").prop("src", "images/empty.png");
}

function handleStock() {
	$("#stock_modal").modal({ backdrop: false });
	allAvailable();
	//console.log(">>> HANDLE STOCK: tu bedzie printowanie kart i klikanie w nie");
}

function useCard(event) {
	var id = Number(event.data.value);

	////////////////////////////
	var test = ">>> USECARD: " + event.data.value;
	console.log(test);

	if (!self.cards[id].available) {
		console.log(">>> USECARD: card is not available");
		return;
	}

	self.cards[id].used = true;
	self.cards[id].available = false;
	++self.used;
	removeCardImg(id);

	// give away cards after getting stock
	if (self.cards.length == 10 && self.used == 2) {
		noneAvailable();
		var second_card = id;
		for (var first_card in self.cards) {
			if (self.cards[first_card].used && first_card != second_card) {
				break;
			}
		}
		let msg = {
			action: "deal",
			data: [{
				player: getLeftPlayerId(), // left player
				card: Number(first_card)
			}, {
				player: getRightPlayerId(), // right player
				card: Number(second_card)
			}]
		}
		console.log(">>> give cards: " + JSON.stringify(msg));
		sendToServer(msg);
		return;
	}

	// play card
	if (self.cards.length == 8 || self.used > 2) {
		noneAvailable();
		var msg = {
			action: "play",
			data: id
		}
		console.log(">>> send play: " + JSON.stringify(msg));
		sendToServer(msg);
	}
}

function noneAvailable() {
	for (let card of self.cards) {
		card.available = false;
	}
}

function allAvailable() {
	for (let card of self.cards) {
		card.available = true;
	}
}

function someAvailable(array) {
	noneAvailable();
	for (let id of array) {
		self.cards[id].available = true;
	}
}

function sendToServer(msg) {
	console.log("vvvvvvvv" + JSON.stringify(msg));
	ws.send(JSON.stringify(msg));
}

function removeCardImg(id) {
	$("#card" + id).remove();
}

/*function drawCard(index) {
	var card = self.cards[index];
	var elem = $("<img/>", {
		id: "card" + index,
		src: "images/" + card.figure + "_" + card.suit + ".svg",
		class: "card",
		title: index
	});
	elem.click({ value: index }, useCard);
	$("#cards").append(elem);
}*/

function path(card) {
	return ("images/" + card.figure + "_" + card.suit + ".svg");
}

function spawnCardImg(card, index) {
	var elem = $("<img/>", {
		id: "card" + index,
		src: path(card),
		class: "card",
		title: index
	});
	elem.click({ value: index }, useCard);
	return elem;
}

function drawCards() {
	var anchor = $("#cards");
	var images = [];
	anchor.html("");
	
	for (let i in self.cards) {
		images.push(spawnCardImg(self.cards[i], i));
	}

	var order = function (suit) {
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

	images.sort(function (first, second) {
		var first_card = self.cards[first.prop("id").charAt(4)];
		var second_card = self.cards[second.prop("id").charAt(4)];

		if (first_card.suit == second_card.suit) {
			return second_card.figure - first_card.figure;
		}

		return order(first_card.suit) - order(second_card.suit);
	});

	for (let elem of images) {
		anchor.append(elem);
	}
}

function getLeftPlayerId() {
	return game[(self.index + 1) % 3].id;
}

function getRightPlayerId() {
	return game[(self.index + 2) % 3].id;
}

function displayCard(prev) {
	if (counter == 3) {
		counter = 0;
		clearTop();
		clearBottom();
	}
	++counter;

	var card = new Card(prev.figure, prev.suit);

	if (prev.player == self.id) {
		$("#bottom_middle").prop("src", path(card));
	}
	else if (prev.player == getLeftPlayerId()) {
		$("#top_left").prop("src", path(card));
	}
	else {
		$("#top_right").prop("src", path(card));
	}
}