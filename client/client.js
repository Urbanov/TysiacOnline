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

class Player {
	constructor(id, nick) {
		this.id = id;
		this.nick = nick;
		this.cards = [];
		this.used = 0;
		this.index = undefined;
		this.score = 0;
		this.bid = 0;
		this.n_cards = 0;
		this.turn = false;
	}
}

class Game {
	constructor() {
		this.players = [];
		this.timeout;
		this.counter = 0;
		this.round = 0;
	}
}

// global variables
var ws;
var self;
var game;

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

			case "end":
				handleEnd(msg);
				break;

			case "leave":
				handleLeave(msg);
				break;

			default:
				console.log(">>> UNUSED: " + JSON.stringify(msg));
				break;
		}
	}
});

function handleWelcome(msg) {
	self.id = msg.data;
}

function handleShow(msg) {
	$("#room_list").html("");
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
	// check if game has just started
	if (game.round == 0) {
		startFirstRound();
	}

	// get cards from server and draw them
	for (let card of msg.data) {
		addCard(card);
	}
	drawCards();

	// change number of opponents' cards and draw them
	if (msg.data.length == 7) {
		for (let player of game.players) {
			player.n_cards = 7;
		}
		drawOpponentsCards();
	}
	else if (msg.data.length == 1) {
		for (let player of game.players) {
			if (player.bid == "pass") {
				++player.n_cards;
			}
			else {
				player.n_cards -= 2;
			}
		}
		drawOpponentsCards();
	}
}

function handleBid(msg) {
	// first bid
	if (self.cards.length != 10 && msg.data.value == 100) {
		teaseStock();
	}

	// final bid
	else if (self.cards.length == 10) {
		for (let player of game.players) {
			if (player.id != self.id) {
				++player.n_cards;
			}
		}
		drawOpponentsCards();
	}

	setTurn(msg.player);
	updateBid(msg.data.id, msg.data.value);
	drawBids();
	if (msg.player == self.id) {
		showBids(msg.data.min, msg.data.max);
	}
}

function handleStock(msg) {
	setTurn(msg.player);
	getPlayerWithId(msg.player).n_cards = 10;
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
	for (let player of game.players) {
		if (player.id == msg.player) {
			updateBid(player.id, msg.data);
		}
		else {
			updateBid(player.id, 0);
		}
	}
	drawBids();
	if (msg.player == self.id) {
		console.log(">>> jedziesz");
		allAvailable();
	}
}

function handlePlay(msg) {
	setTurn(msg.player);
	rotate();

	if (msg.data.prev.marriage) {
		drawTrump(msg.data.prev.suit);
	}

	displayCard(msg.data.prev);
	if (self.id == msg.player) {
		console.log(">>> jedziesz");
		someAvailable(msg.data.available);
	}
}

function handleScore(msg) {
	for (let data of msg.data) {
		updateScore(data.player, data.score);
	}
	drawScores();
	newRound();
}

function handleEnd(msg) {
	for (let player of game.players) {
		player.score = 0;
	}
	game.round = 0;
	var text = getPlayerWithId(msg.player) + " wins the game with " + msg.data + " points!";
	$("#win_modal .modal-title").text(text);
	$("#win_modal").modal({ backdrop: false });
}

function handleLeave(msg) {
	$("#ready_modal").modal("hide");
	addMessage(getPlayerWithId(msg.data).nick + " has left the room");
	for (let i in game.players) {
		if (game.players[i].id == msg.data) {
			game.players.splice(i, 1);
			break;
		}
	}
	clearCardData();
	self.index = (game.players[0].id == self.id ? 0 : 1);
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
	$("#login_modal").modal({ backdrop: "static" }); //FIXME
	$("#login_modal").on("shown.bs.modal", function () {
		$("#nickname").focus();
	});
	$("#login").click(login);
	$("#ready").click(sendReady);
	$("#win_modal .btn").click(function () {
		$("#win_modal").modal("hide");
		clearTable();
		hideModals();
		checkReady();
	});
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
	var msg = {
		action: "leave"
	};
	sendToServer(msg);
	clearTable();
	clearChat();
	hideModals();
	game = null;
	requestRefresh();
	$("#game_panel").hide();
	$("#lobby").show();
}

function sendMessage() {
	var anchor = $("#text_area");
	if (anchor.val() == "") {
		return;
	}
	var text = self.nick + ": " + anchor.val();
	anchor.val("");
	addMessage(text);
	var msg = {
		action: "chat",
		data: text
	};
	sendToServer(msg);
}

function addMessage(msg) {
	$("#chatbox").append($("<div/>", {
		text: "\xBB " + msg
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
	game = new Game();
	$("#lobby").hide();
	$("#game_panel").show();
	for (let player of data) {
		addPlayer(player);
	}
	self.index = game.players.length - 1;
}

function addPlayer(player) {
	if (player.id == self.id) {
		game.players.push(self);
	}
	else {
		game.players.push(new Player(player.id, player.nick));
	}
	checkReady();
}

function showBids(min, max) {
	$("#bids").html("");

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

	$("#bids_modal").modal({ backdrop: false });
	$(".modal-backdrop").appendTo("#game_canvas");
}

function sendBid(event) {
	$('#bids_modal').modal("hide");
	var msg = {
		action: "bid",
		data: event.data.value
	};
	sendToServer(msg);
}

function loadRooms(data) {
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
	clearTable();
	$("#ready_modal").modal({ backdrop: false });
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
	for (let player of game.players) {
		if (player.id == target) {
			player.bid = value != -1 ? value : "pass";
			break;
		}
	}
}

function updateScore(target, value) {
	getPlayerWithId(target).score += value;
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
	--self.n_cards;
	removeCardImg(id);

	// give away cards after getting stock
	if (self.cards.length == 10 && self.used == 2) {
		giveAwayCards(id);
		return;
	}

	// play card
	if (self.cards.length == 8 || self.used > 2) {
		playCard(id);
	}
}

function giveAwayCards(second_card) {
	noneAvailable();
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
	};
	console.log(">>> give cards: " + JSON.stringify(msg));
	sendToServer(msg);
}

function playCard(id) {
	noneAvailable();
	var msg = {
		action: "play",
		data: id
	};
	console.log(">>> send play: " + JSON.stringify(msg));
	sendToServer(msg);
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
	return game.players[(self.index + 1) % 3];
}

function getRightPlayer() {
	return game.players[(self.index + 2) % 3];
}

function displayCard(prev) {
	var card = new Card(prev.figure, prev.suit);

	if (prev.player == self.id) {
		$("#bottom_middle").prop("src", path(card));
		return;
	}

	if (prev.player == getLeftPlayer().id) {
		$("#top_left").prop("src", path(card));
		$("#opponent_left").prop("src", "images/opponent" + --getLeftPlayer().n_cards + ".png");
		return;
	}

	$("#top_right").prop("src", path(card));
	$("#opponent_right").prop("src", "images/opponent" + --getRightPlayer().n_cards + ".png");
}

function newRotation() {
	game.counter = 0;
	game.timeout = setTimeout(function () {
		clearTop();
		clearBottom();
	}, 2000);
}

function teaseStock() {
	clearTimeout(game.timeout);
	drawTrump(0);
	clearTop();
	$("#bottom_left").prop("src", "images/back.png");
	$("#bottom_middle").prop("src", "images/back.png");
	$("#bottom_right").prop("src", "images/back.png");
}

function drawOpponentsCards() {
	$("#opponent_left").prop("src", "images/opponent" + getLeftPlayer().n_cards + ".png");
	$("#opponent_right").prop("src", "images/opponent" + getRightPlayer().n_cards + ".png");
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

function setTurn(id) {
	console.log(">>> SETTURN: " + id);
	for (let player of game.players) {
		if (player.id == id) {
			
			player.turn = true;
		}
		else {
			player.turn = false;
		}
	}
	drawTurns();
}

function drawTurns() {
	console.log(" >>> drawing turns");
	$("#data_left .nick").css({
		"color": getLeftPlayer().turn ? "#fff" : "#bbb",
		"font-weight": getLeftPlayer().turn ? "bold" : "normal"
	});
	$("#data_middle .nick").css({
		"color": self.turn ? "#fff" : "#bbb",
		"font-weight": self.turn ? "bold" : "normal"
	});
	$("#data_right .nick").css({
		"color": getRightPlayer().turn ? "#fff" : "#bbb",
		"font-weight": getRightPlayer().turn ? "bold" : "normal"
	});
}

function drawPlayers() {
	$("#data_left .nick").text(getLeftPlayer().nick);
	$("#data_middle .nick").text(self.nick);
	$("#data_right .nick").text(getRightPlayer().nick);
}

function drawScores() {
	$("#data_left .score").text("score: " + getLeftPlayer().score);
	$("#data_middle .score").text("score: " + self.score);
	$("#data_right .score").text("score: " + getRightPlayer().score);
}

function drawBids() {
	$("#data_left .bid").text(getLeftPlayer().bid ? "bid: " + getLeftPlayer().bid : "");
	$("#data_middle .bid").text(self.bid ? "bid: " + self.bid : "");
	$("#data_right .bid").text(getRightPlayer().bid ? "bid: " + getRightPlayer().bid : "");
}

function drawTrump(suit) {
	if (suit == 0) {
		$("#data_middle .trump").css("visibility", "hidden");
	}
	else {
		$("#trump").prop("src", "images/" + suit + ".png");
		$("#data_middle .trump").css("visibility", "visible");
	}
}

function startFirstRound() {
	for (let player of game.players) {
		player.score = 0;
	}
	clearCardData();
	drawPlayers();
	drawScores();
	++game.round;
}

function getPlayerWithId(id) {
	for (let player of game.players) {
		if (player.id == id) {
			return player;
		}
	}
}

function rotate() {
	++game.counter;
	if (game.counter == 1) {
		clearTimeout(game.timeout);
		clearBottom();
		clearTop();
	}
	else if (game.counter == 3) {
		newRotation();
	}
}

function newRound() {
	clearCardData();
	for (let player of game.players) {
		player.bid = 0;
	}
	++game.round;
}

function clearCardData() {
	self.cards = [];
	self.used = 0;
}

function checkReady() {
	// room is full
	if (game.players.length == 3) {
		askReady();
	}
}

function clearTable() {
	drawTrump(0);
	clearTop();
	clearBottom();
	$(".nick").text("");
	$(".score").text("");
	$(".bid").text("");
	$(".opponent_cards img").prop("src", "images/empty.png");
	$("#cards").html("");
}

function hideModals() {
	$(".modal").modal("hide");
}

function clearChat() {
	$("#chatbox").html("");
}

function test(i) {
	handleLeave({action: "leave", data: i});
}