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
				console.log(msg.data);
				updateBid(msg.data.id, msg.data.value);
				if (msg.player == self.id) {
					showBids(msg.data.min, msg.data.max);
				}
				break;

			case "stock":
				debug(msg);
				displayStock(msg.data);
				if (msg.player == self.id) {
					for (let card of msg.data) {
						addCard(card);
					}
					handleStock();
				}
				break;

			default:
				// ready is unused for now
				console.log("ready: " + JSON.stringify(msg));
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

	var elem = $("<button/>", {
		text: "pass",
		class: "btn btn-default btn-danger"
	});
	elem.click({ value: -1 }, sendBid);
	$("#bids").append(elem);


	for (var bid = 110; bid < min; bid += 10) {
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
	//FIXME
	console.log(">>> stock: " + JSON.stringify(cards));
}

function handleStock() {
	for (let i in game) {
		if (game[i].id != self.id) {
			console.log(">>> give card to " + game[i].nick + " (id: " + game[i].id + "): ");

		}
	}
}

function useCard(id) {
	self.cards[id].used = true;
	++self.used;

	// 
	if (self.cards.length == 10 && self.used == 2) {
		var second_card = id;
		for (var first_card in self.cards) {
			if (self.cards[first_card].used && first_card != second_card) {
				break;
			}
		}
		var msg = {
			action: "deal",
			data: [{
				player: (self.index + 1) % 3, // next player
				card: first_card
			}, {
				player: (self.index + 2) % 3, // previous player
				card: second_card
			}]
		}

		debug(msg);
		ws.send(JSON.stringify(msg));
	}
}

function debug(what) {
	console.log(JSON.stringify(what));
}