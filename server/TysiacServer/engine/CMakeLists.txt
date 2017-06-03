set(engine_sources
	Adder.cpp
	Bidder.cpp
	Card.cpp
	ChatBox.cpp
	Controller.cpp
	Dealer.cpp
	Deck.cpp
	Game.cpp
	GameManager.cpp
	LeaveBuster.cpp
	Player.cpp
	PlayersCollection.cpp
	PlayersDeck.cpp
	Room.cpp
	Score.cpp
	Starter.cpp
	SumScore.cpp
)

add_library(engine ${engine_sources})