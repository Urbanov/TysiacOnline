var room_id;
var player_id;
var ws = new WebSocket("ws://127.0.0.1:2137");

ws.onmessage = function (event) {
    var msg = JSON.parse(event.data);
    switch (msg.action) {
        case "chat":
            $("#chatbox").append(msg.player + ": " + msg.values + "<br>");
            break;

        case "welcome":
            player_id = msg.values;
            $("#player_id").append(msg.values);
            break;

        case "add":
            room_id = msg.id;
            $("#game_id").append(msg.id);
            break;
    }
}

function joinRoom() {
    var msg = {
        action: "add",
        player: player_id,
        values: "placeholder"
    };
    ws.send(JSON.stringify(msg));
}

function sendMessage() {
    var msg = {
        action: "chat",
        player: player_id,
        values: $("#text_area").val(),
        id: room_id
    };
    $("#text_area").val("");
    $("#chatbox").append(msg.player + ": " + msg.values + "<br>");
    ws.send(JSON.stringify(msg));
}

$("#submit").click(sendMessage);

$("#connect").click(function () {
    $(this).attr("disabled", true);
    joinRoom();
})

$("#disconnect").click(function () {
    ws.close();
})


