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

	// initiate event listeners
	initiateEventListeners();

	// open a connection
	ws = new WebSocket("ws://127.0.0.1:2137");

	// refresh server list
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
				handleWelcome(msg);
				break;

			case "show":
				handleShow(msg);
				break;

			case "add":
				handleAdd(msg);
				break;

			case "new_player":
				handleNewPlayer(msg);
				break;

			case "chat":
				handleChat(msg);
				break;

			case "deal":
				handleDeal(msg);
				break;

			case "bid":
				handleBid(msg);
				break;

			case "stock":
				handleStock(msg);
				break;

			case "start":
				handleStart(msg);
				break;

			case "play":
				handlePlay(msg);
				break;

			case "score":
				handleScore(msg);
				break;

			default:
				// ready is unused for now
				//console.log(">>> UNUSED: " + JSON.stringify(msg));
				break;
		}
	}
});

function handleWelcome(msg) {
	self.id = msg.data;
}

function handleShow(msg) {
	if (msg.hasOwnProperty("data")) {
		loadRooms(msg.data);
	}
}

function handleAdd(msg) {
	if (!msg.error) {
		$("#join_error").hide();
		joinRoom(msg.data);
	}
	else {
		$("#join_error").show();
		requestRefresh();
	}
}

function handleNewPlayer(msg) {
	addMessage(msg.data.nick + " has entered the room");
	addPlayer(msg.data);
}

function handleChat(msg) {
	addMessage(msg.data);
}

function handleDeal(msg) {
	for (let card of msg.data) {
		addCard(card);
	}

	drawCards();

	if (msg.data.length == 7) {
		for (let player of game) {
			player.cards = 7;
		}
		drawOpponentsCards();
		return;
	}

	if (msg.data.length == 1) {
		for (let player of game) {
			if (player.bid == "pass") {
				++player.cards;
			}
			else {
				player.cards -= 2;
			}
		}
		drawOpponentsCards();
	}
}

function handleBid(msg) {
	if (self.cards.length != 10 && msg.data.value == 100) {
		teaseStock();
	}
	else if (self.cards.length == 10) {
		for (let player of game) {
			if (player.id != self.id) {
				++player.cards;
			}
		}
		drawOpponentsCards();
	}
	updateBid(msg.data.id, msg.data.value);
	if (msg.player == self.id) {
		showBids(msg.data.min, msg.data.max);
	}
}

function handleStock(msg) {

	for (player of game) {
		if (player.id == msg.player) {
			player.cards = 10;
		}
	}
	drawOpponentsCards();

	displayStock(msg.data);
	if (msg.player == self.id) {
		for (let card of msg.data) {
			addCard(card);
		}
		drawCards();
		$("#stock_modal").modal({ backdrop: false });
		allAvailable();
	}
}

function handleStart(msg) {
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
}

function handlePlay(msg) {
	displayCard(msg.data.prev);
	if (self.id == msg.player) {
		console.log(">>> jedziesz");
		someAvailable(msg.data.available);
	}
	// display card
	//console.log(">>> DISPLAY: " + JSON.stringify(msg.data.prev));
}

function handleScore(msg) {
	self.cards = [];
	self.used = 0;
}

function initiateEventListeners() {
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
}









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

function updateBid(target, value) {
	for (let player of game) {
		if (player.id == target) {
			player.bid = value != -1 ? value : "pass";
			break;
		}
	}
}

function displayStock(cards) {
	clearTop();
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
	--game[self.index].cards;
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
				player: getLeftPlayer().id, // left player
				card: Number(first_card)
			}, {
				player: getRightPlayer().id, // right player
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

function getLeftPlayer() {
	return game[(self.index + 1) % 3];
}

function getRightPlayer() {
	return game[(self.index + 2) % 3];
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
		return;
	}

	var left_player = getLeftPlayer();
	if (prev.player == left_player.id) {
		$("#top_left").prop("src", path(card));
		$("#opponent_left").prop("src", "images/opponent" + --left_player.cards + ".png");
		return;
	}

	var right_player = getRightPlayer();
	$("#top_right").prop("src", path(card));
	$("#opponent_right").prop("src", "images/opponent" + --right_player.cards + ".png");
}

function teaseStock() {
	clearTop();
	$("#bottom_left").prop("src", "images/back.png");
	$("#bottom_middle").prop("src", "images/back.png");
	$("#bottom_right").prop("src", "images/back.png");
}

function drawOpponentsCards() {
	$("#opponent_left").prop("src", "images/opponent" + getLeftPlayer().cards + ".png");
	$("#opponent_right").prop("src", "images/opponent" + getRightPlayer().cards + ".png");
}

function order(suit) {
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