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
	ws = new WebSocket("ws://127.0.0.1:2137");

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
				self.id = msg.values;
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
				}
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
					}
					handleStock();
				}
				break;

			case "start":
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
				}
				break;

			case "play":
				if (self.id == msg.player) {
					console.log(">>> jedziesz");
				}
				// display card
				console.log(">>> DISPLAY: " + JSON.stringify(msg.data.prev));
				break;

			default:
				// ready is unused for now
				console.log(">>> UNUSED: " + JSON.stringify(msg));
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
	ws.send(JSON.stringify(msg));
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
	ws.send(JSON.stringify(msg));
}

function requestRoom(event) {
	var msg = {
		action: "add",
		data: self.nick,
		id: event.data.id
	};
	ws.send(JSON.stringify(msg));
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
	debug(msg);
	ws.send(JSON.stringify(msg));
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
	ws.send(JSON.stringify(msg));
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
	//TODO
	//console.log(">>> stock: " + JSON.stringify(cards));
}

function handleStock() {
	$("#stock_modal").modal({ backdrop: false });
	console.log(">>> HANDLE STOCK: tu bedzie printowanie kart i klikanie w nie");
}

function useCard(id) {
	self.cards[id].used = true;
	++self.used;

	// give away cards after getting stock
	if (self.cards.length == 10 && self.used == 2) {
		var second_card = id;
		for (var first_card in self.cards) {
			if (self.cards[first_card].used && first_card != second_card) {
				break;
			}
		}
		let msg = {
			action: "deal",
			data: [{
				player: game[(self.index + 1) % 3].id, // next player
				card: Number(first_card)
			}, {
				player: game[(self.index + 2) % 3].id, // previous player
				card: Number(second_card)
			}]
		}
		ws.send(JSON.stringify(msg));
		return;
	}

	// play card
	if (self.cards.length == 8 || self.used > 2) {
		var msg = {
			action: "play",
			data: id
		}
		console.log(">>> send play: " + JSON.stringify(msg));
		ws.send(JSON.stringify(msg));
	}
}

function makeAvailable(array) {
	for (let card of array);
}

function debug(what) {
	console.log(JSON.stringify(what));
}