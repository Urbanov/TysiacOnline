set(engine_sources
	adder.cpp
	bidder.cpp
	card.cpp
	chat_box.cpp
	controller.cpp
	dealer.cpp
	deck.cpp
	game.cpp
	game_manager.cpp
	leave_buster.cpp
	player.cpp
	players_collection.cpp
	players_deck.cpp
	room.cpp
	score.cpp
	starter.cpp
	sum_score.cpp
)

add_library(engine ${engine_sources})