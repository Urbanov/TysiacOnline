var Modals = {
	BID: "#bids_modal",
	READY: "#ready_modal",
	STOCK: "#stock_modal",
	WIN: "#win_modal"
};

class View {
	askReady() {
		this.showModal(Modals.READY);
	}

	displayStock(cards) {
		this.clearTop();
		$("#bottom_left").prop("src", this.path(cards[0]));
		$("#bottom_middle").prop("src", this.path(cards[1]));
		$("#bottom_right").prop("src", this.path(cards[2]));
	}

	clearBottom() {
		$("#bottom_left").prop("src", "images/empty.png");
		$("#bottom_middle").prop("src", "images/empty.png");
		$("#bottom_right").prop("src", "images/empty.png");
	}

	clearTop() {
		$("#top_left").prop("src", "images/empty.png");
		$("#top_right").prop("src", "images/empty.png");
	}

	removeCardImg(id) {
		$("#card" + id).remove();
	}

	path(card) {
		return ("images/" + card.figure + "_" + card.suit + ".svg");
	}

	spawnCardImg(card, index) {
		var elem = $("<img/>", {
			id: "card" + index,
			src: this.path(card),
			class: "card"
		});
		elem.click({ value: index }, $.proxy(controller.self.useCard, controller.self));
		return elem;
	}

	drawCards() {
		var anchor = $("#cards");
		var images = [];
		anchor.html("");

		for (let i in controller.self.cards) {
			images.push(this.spawnCardImg(controller.self.cards[i], i));
		}

		images.sort(function (first, second) {
			var first_card = controller.self.cards[first.prop("id").charAt(4)];
			var second_card = controller.self.cards[second.prop("id").charAt(4)];

			if (first_card.suit == second_card.suit) {
				return second_card.figure - first_card.figure;
			}

			return Card.order(first_card.suit) - Card.order(second_card.suit);
		});

		for (let elem of images) {
			anchor.append(elem);
		}
	}

	displayCard(prev) {
		var card = new Card(prev.figure, prev.suit);

		if (prev.player == controller.self.id) {
			$("#bottom_middle").prop("src", this.path(card));
			return;
		}

		if (prev.player == controller.self.getLeftPlayer().id) {
			$("#top_left").prop("src", this.path(card));
			$("#opponent_left").prop("src", "images/opponent" + --controller.self.getLeftPlayer().n_cards + ".png");
			return;
		}

		$("#top_right").prop("src", this.path(card));
		$("#opponent_right").prop("src", "images/opponent" + --controller.self.getRightPlayer().n_cards + ".png");
	}

	teaseStock() {
		clearTimeout(controller.game.timeout);
		this.drawTrump(Suits.NONE);
		this.clearTop();
		$("#bottom_left").prop("src", "images/back.png");
		$("#bottom_middle").prop("src", "images/back.png");
		$("#bottom_right").prop("src", "images/back.png");
	}

	drawOpponentsCards() {
		$("#opponent_left").prop("src", "images/opponent" + controller.self.getLeftPlayer().n_cards + ".png");
		$("#opponent_right").prop("src", "images/opponent" + controller.self.getRightPlayer().n_cards + ".png");
	}

	drawTurns() {
		$("#data_left .nick").css({
			"color": controller.self.getLeftPlayer().turn ? "#fff" : "#bbb",
			"font-weight": controller.self.getLeftPlayer().turn ? "bold" : "normal"
		});
		$("#data_middle .nick").css({
			"color": controller.self.turn ? "#fff" : "#bbb",
			"font-weight": controller.self.turn ? "bold" : "normal"
		});
		$("#data_right .nick").css({
			"color": controller.self.getRightPlayer().turn ? "#fff" : "#bbb",
			"font-weight": controller.self.getRightPlayer().turn ? "bold" : "normal"
		});
	}

	drawPlayers() {
		$("#data_left .nick").text(controller.self.getLeftPlayer().nick);
		$("#data_middle .nick").text(controller.self.nick);
		$("#data_right .nick").text(controller.self.getRightPlayer().nick);
	}

	drawScores() {
		$("#data_left .score").text("score: " + controller.self.getLeftPlayer().score);
		$("#data_middle .score").text("score: " + controller.self.score);
		$("#data_right .score").text("score: " + controller.self.getRightPlayer().score);
	}

	drawBids() {
		$("#data_left .bid").text(controller.self.getLeftPlayer().bid ? "bid: " + controller.self.getLeftPlayer().bid : "");
		$("#data_middle .bid").text(controller.self.bid ? "bid: " + controller.self.bid : "");
		$("#data_right .bid").text(controller.self.getRightPlayer().bid ? "bid: " + controller.self.getRightPlayer().bid : "");
	}

	drawTrump(suit) {
		if (suit == Suits.NONE) {
			$("#data_middle .trump").css("visibility", "hidden");
		}
		else {
			$("#trump").prop("src", "images/" + suit + ".png");
			$("#data_middle .trump").css("visibility", "visible");
		}
	}

	clearTable() {
		this.drawTrump(Suits.NONE);
		this.clearTop();
		this.clearBottom();
		$(".nick").text("");
		$(".score").text("");
		$(".bid").text("");
		$(".opponent_cards img").prop("src", "images/empty.png");
		$("#cards").html("");
	}

	hideModals() {
		$(".modal.in").modal("hide");
	}

	clearChat() {
		$("#chatbox").html("");
	}

	changeScene() {
		$("#game_panel").toggle();
		$("#lobby").toggle();
	}

	addMessage(msg) {
		$("#chatbox").append($("<div/>", {
			text: "\xBB " + msg
		}));
	}

	showBids(min, max) {
		$("#bids").html("");

		// normal bid (includes pass button)
		if (controller.self.cards.length != 10) {
			var bid = 110;
			var elem = $("<button/>", {
				text: "pass",
				class: "btn btn-default btn-danger"
			});
			elem.click({ value: -1 }, $.proxy(controller.sendBid, controller));
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
			elem.click({ value: bid }, $.proxy(controller.sendBid, controller));
			$("#bids").append(elem);
		}

		for (; bid <= 360; bid += 10) {
			var elem = $("<button/>", {
				text: bid,
				class: "btn btn-default bid_button disabled"
			});
			$("#bids").append(elem);
		}

		this.showModal(Modals.BID);
		$(".modal-backdrop").appendTo("#game_canvas");
	}

	getChatMessage() {
		var anchor = $("#text_area");
		var text = anchor.val();
		anchor.val("");
		return text;
	}

	drawEnd(text) {
		$("#win_modal .modal-title").text(text);
		this.showModal(Modals.WIN);
	}

	showLoginError() {
		$("#login_error").show();
	}

	showJoinError() {
		$("#join_error").show();
	}

	hideJoinError() {
		$("#join_error").hide();
	}

	getNickname() {
		return $("#nickname").val();
	}

	drawRoom(room) {
		var players = room.nick.join(", ");
		var elem = $("<button/>", {
			text: players,
			class: "room btn btn-block btn-default"
		});
		if (room.nick.length < 3) {
			elem.click({ id: room.id }, $.proxy(controller.requestRoom, controller));
		}
		else {
			elem.prop("disabled", true);
		}
		$("#room_list").append(elem);
	}

	clearRoomList() {
		$("#room_list").html("");
	}

	initiateEventListeners() {
		$("#create_room").click({ id: -1 }, $.proxy(controller.requestRoom, controller));
		$("#refresh_list").click($.proxy(controller.requestRefresh, controller));
		$("#send").click($.proxy(controller.sendMessage, controller));
		$("#text_area").keypress($.proxy(function (event) {
			if (event.keyCode == 13) {
				event.preventDefault();
				controller.sendMessage();
			}
		}, controller));
		$("#leave").click($.proxy(controller.leaveRoom, controller));
		$("#login_modal").modal({ backdrop: "static" });
		$("#login_modal").on("shown.bs.modal", function () {
			$("#nickname").focus();
		});
		$("#login").keypress($.proxy(function (event) {
			if (event.keyCode == 13) {
				controller.login();
			}
		}, controller));
		$("#login").click($.proxy(controller.login, controller));
		$("#ready").click($.proxy(controller.sendReady, controller));
		$("#win_modal .btn").click($.proxy(function () {
			this.hideModals();
			this.clearTable();
			this.hideModals();
			controller.game.checkReady();
		}, this));
	}

	showModal(modal_id) {
		$(modal_id).modal({ backdrop: false });
	}
}