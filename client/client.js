class Player {
	constructor(id) {
		this.id = id;
	}
}

var room_id;
var player_id;
var player_nick;
var ws;

$(document).ready(function () {
	ws = new WebSocket("ws://127.0.0.1:2137");

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

	$("#login_modal").modal({ backdrop: "static" });
	$("#login").click(login);
	$("#ready").click(sendReady);


	/* on open */
	//requestRefresh();

	/*ZMIENIC*/
	ws.onmessage = function (event) {
		var msg = JSON.parse(event.data);
		switch (msg.action) {
			case "welcome":
				player_id = msg.values;
				break;

			case "show":
				if (msg.hasOwnProperty("data")) {
					loadRooms(msg.data);
				}
				break;

			case "add":
				alert("add: " + JSON.stringify(msg.data));
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
				alert("new_player: " + JSON.stringify(msg.data));
				break;

			case "chat":
				addMessage(msg.data);
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
		player_nick = nick;
	}
}

function leaveRoom() {
	alert("xD");
	$("#lobby").show();
	$("#game_panel").hide();
}

function sendMessage() {
	var text = player_nick + ": " + $("#text_area").val();
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
		data: player_nick,
		id: event.data.id
	};
	ws.send(JSON.stringify(msg));
}

function joinRoom(data) {
	$("#lobby").hide();
	$("#game_panel").show();
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

	var bid = 110;

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
	alert(event.data.value);
}

function loadRooms(data) {
	$("#room_list").html("");
	for (room of data) {
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
	alert("ready");
}