#include "controller.hpp"

Controller::Controller(Deck& deck, PlayersCollection& players)
	: deck_(deck)
	, players_(players)
{}

Controller::~Controller()
{}

void Controller::reset()
{}
