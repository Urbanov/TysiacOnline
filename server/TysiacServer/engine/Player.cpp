#include "Player.hpp"

Player::Player(int player_id, std::string & nick)
	: ready_(false)
	, player_id_(player_id)
	, nick_(nick)
{}

Player::Player(const Player & other)
	: ready_(other.ready_)
	, score_(other.score_)
	, player_id_(other.player_id_)
	, nick_(other.nick_)
	, players_deck_(other.players_deck_)
{}

Player& Player::operator=(const Player & other)
{
	ready_ = other.ready_;
	score_ = other.score_;
	player_id_ = other.player_id_;
	nick_ = other.nick_;
	players_deck_ = other.players_deck_;
	return *this;
}

Player::~Player() {}

bool Player::operator==(const Player & other) const
{
	return (player_id_ == other.player_id_);
}

PlayersDeck & Player::getPlayersDeck()
{
	return players_deck_;
}

size_t Player::getPlayerId() const
{
	return player_id_;
}

const std::string & Player::getPlayersNick() const
{
	return nick_;
}

Score & Player::getScoreClass()
{
	return score_;
}

bool Player::getReady() const
{
	return ready_;
}

void Player::setReady(bool ready)
{
	ready_ = ready;
}

