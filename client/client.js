class Player {
	constructor(id) {
		this.id = id;
	}
}

var room_id;
var player_id;
var ws;

$(document).ready(function () {
	ws = new WebSocket("ws://127.0.0.1:2137");
	var js = [{ "id": 0, "nicks": ["1", "2", "3"] }];
	loadRooms(js);

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

	/*ZMIENIC*/
	ws.onmessage = function (event) {
		var msg = JSON.parse(event.data);
		switch (msg.action) {
			case "welcome":
				player_id = msg.values;
				break;

			case "add":
				room_id = msg.id;
				joinRoom();
				break;
		}
	}

});

function login() {
	var nick = $("#nickname").val();
	if (!nick.length) {
		$(".alert").show();
	}
	else {
		$("#login_modal").modal("hide");
		alert(nick);
	}
}

function leaveRoom() {
	alert("xD");
	$("#lobby").show();
	$("#game_panel").hide();
}

function sendMessage() {
	var msg = $("#text_area").val();
	$("#text_area").val("");
	addMessage(msg);
}

function addMessage(msg) {
	$('#chatbox').append($("<div>", {
		text: msg
	}));
}

function requestRefresh() {
	//ws.send(refresh);
	var js = [{ "id": 0, "nicks": ["1", "2", "3"] }, { "id": 1, "nicks": ["4", "5", "6"] }];
	loadRooms(js);
}

function requestRoom(event) {
	//alert(event.data.id);

	/*ZMIENIC*/
	var msg = {
		action: "add",
		player: player_id,
		values: "placeholder"
	};
	ws.send(JSON.stringify(msg));

	//ws.send(id);
}

function joinRoom() {
	$("#lobby").hide();
	$("#game_panel").show();
	//showModal();
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
		var players = room.nicks.join(", ");
		var elem = $("<button/>", {
			text: players,
			class: "room btn btn-block btn-default"
		});
		elem.click( {id: room.id }, requestRoom)
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