#include "GameLogic.hpp"

// <Clas Card>

Card::Card(figures fig, suits s)
	: figure_(fig)
	, suit_(s)
	, is_used_(false) 
{}

Card::Card(const Card & other)
	: figure_(other.figure_) 
	, suit_(other.suit_)
	, is_used_(other.is_used_) 
{}

Card::Card(Card && other)
	: figure_(std::move(other.figure_)) 
	, suit_(std::move(other.suit_))
	, is_used_(std::move(other.is_used_)) 
{}

Card Card::operator=(const Card & card)
{
	this->figure_ = card.figure_;
	this->suit_ = card.suit_;
	this->is_used_ = card.is_used_;
	return *this;
}

Card Card::operator=(Card && card)
{
	this->figure_ = card.figure_;
	this->suit_ = card.suit_;
	this->is_used_ = card.is_used_;
	return *this;
}

Card::~Card() {}

bool Card::getIsUsed() const
{
	return is_used_;
}

void Card::setIsUsed(bool is_used) const
{
	is_used_ = is_used;
}

const figures Card::getFigure() const
{
	return figure_;
}

const suits Card::getSuit() const
{
	return suit_;
}

// </Class Card>



// <Clas PlayerDeck>

PlayerDeck::PlayerDeck()
	: max_value_(0)
{}

PlayerDeck::~PlayerDeck() {}

std::vector<Card>& PlayerDeck::getDeck()
{
	return deck_;
}

void PlayerDeck::addCard(const Card & card)
{
	deck_.push_back(card);
	deck_.back().setIsUsed(false);
}

const Card & PlayerDeck::playCard(std::size_t card_number) const
{
	if (card_number >= deck_.size()) {
		throw std::out_of_range("No card with such index in player's deck");
	}
	if (deck_[card_number].getIsUsed()) {
		throw std::logic_error("Card has already been used, non existing");
	}
	deck_[card_number].setIsUsed(true);
	return deck_[card_number];
}

bool PlayerDeck::doesHavePair(suits suit)
{
	return (findCard(KING, suit) || findCard(QUEEN, suit));
}

std::vector<int> PlayerDeck::getAllValidCards(std::vector<Card> & vec, suits superior)
{
	std::vector<int> correct_cards, tmp;
	if (vec.empty()) {
		for (size_t i = 0; i < deck_.size(); ++i) {
			if (!deck_[i].getIsUsed()) {
				correct_cards.push_back(i);
			}
		}
		return correct_cards;
	}
	Card card = vec[0];
	if (vec.size() == 2 && isHigher(vec[0], vec[1], superior)) {
		card = vec[1];
	}
	for (size_t i = 0; i < deck_.size(); ++i) {
		if (isHigher(card, deck_[i], superior) || 
			(vec[0].getSuit() == deck_[i].getSuit() && !deck_[i].getIsUsed())) {
			correct_cards.push_back(i);
		}
	}
	if (correct_cards.empty()) {
		for (size_t i = 0; i < deck_.size(); ++i) {
			if (!deck_[i].getIsUsed()) {
				correct_cards.push_back(i);
			}
		}
		return correct_cards;
	}
	if (vec[0].getFigure() == card.getFigure() && (vec[0].getSuit() == card.getSuit())
		|| vec[0].getSuit() == vec[1].getSuit()) {
		for (const auto & i : correct_cards) {
			if (isHigher(card, deck_[i], superior) && card.getSuit() == deck_[i].getSuit()) {
				tmp.push_back(i);
			}
		}
		if (!tmp.empty()) {
			return tmp;
		}
		else {
			for (const auto & i : correct_cards) {
				if (!deck_[i].getIsUsed() && card.getSuit() == deck_[i].getSuit()) {
					tmp.push_back(i);
				}
			}
		}
		if (!tmp.empty()) {
			return tmp;
		}
		else {
			for (const auto & i : correct_cards) {
				if (isHigher(card, deck_[i], superior)) {
					tmp.push_back(i);
				}
			}
		}
		return tmp;
	}
	for (const auto & i : correct_cards) {
		if (!deck_[i].getIsUsed() && vec[0].getSuit() == deck_[i].getSuit()) {
			tmp.push_back(i);
		}
	}
	if (!tmp.empty()) {
		return tmp;
	}
	else {
		for (const auto & i : correct_cards) {
			if (isHigher(card, deck_[i], superior)) {
				tmp.push_back(i);
			}
		}
		return tmp;
	}
}

std::size_t PlayerDeck::getMaxValue(bool isLateBid)
{
	if (max_value_ != 0 && !isLateBid) {
		return max_value_;
	}
	max_value_ = 0;
	max_value_ += MIN_VALUE;
	std::vector <std::pair<suits, figures> > vec = { 
		{DIAMONDS, NOT_A_FIGURE}, 
		{CLUBS, NOT_A_FIGURE}, 
		{HEARTS,NOT_A_FIGURE},
		{SPADES, NOT_A_FIGURE} 
	};
	for (auto i : deck_) {
		if (i.getFigure() == QUEEN || i.getFigure() == KING) {
			for (auto& j : vec) {
				if (j.first == i.getSuit()) {
					if (j.second != NOT_A_FIGURE) {
						max_value_ += j.first;
					}
					else {
						j.second = i.getFigure();
					}
				}
			}
		}
	}
	return max_value_;
}

bool PlayerDeck::findCard(figures figure, suits suit) const
{
	for (auto& i : deck_) {
		if (i.getFigure() == figure && i.getSuit() == suit && i.getIsUsed() == false) {
			return true;
		}
	}
	return false;
}

bool PlayerDeck::isHigher(const Card & played, const Card deck_card, suits superior)
{
	if (deck_card.getIsUsed()) {
		return false;
	}
	if (played.getSuit() == deck_card.getSuit()) {
		return (played.getFigure() < deck_card.getFigure());
	}
	if (played.getSuit() == superior) {
		return false;
	}
	if (deck_card.getSuit() == superior) {
		return true;
	}
	return false;
}

void PlayerDeck::clearDeck()
{
	deck_.clear();
	max_value_ = 0;
}

// </Class PlayerDeck>



// <Class Score>

Score::Score() 
	: score_(0)
	, temp_score_(0)
	, claim_(0)
{}

Score::~Score()
{}

int Score::getScore() const
{
	return score_;
}

void Score::addScore(int score)
{
	score_ += score;
}

void Score::addToTurnScore(int temp_score)
{
	temp_score_ += temp_score;
}

int Score::getTurnScore() const
{
	return temp_score_;
}

void Score::clearTurnScore()
{
	temp_score_ = 0;
}

bool Score::setClaim(int claim, bool isFinal)
{
	if (isFinal) {
		if (claim >= claim_) {
			claim_ = claim;
			return true;
		}
	}
	if (claim > claim_ || claim == -1) {
		claim_ = claim;
		return true;
	}
	else throw std::logic_error("Trying to claim lower or equal");
}

int Score::getClaim() const
{
	return claim_;
}
void Score::resetClaim()
{
	claim_ = 0;
}

// </Class Score>



// <Class Player>

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
	, player_deck_(other.player_deck_)
{}

Player& Player::operator=(const Player & other)
{
	ready_ = other.ready_;
	score_ = other.score_;
	player_id_ = other.player_id_;
	nick_ = other.nick_;
	player_deck_ = other.player_deck_;
	return *this;
}

Player::~Player() {}

bool Player::operator==(const Player & other) const
{
	return (player_id_ == other.player_id_);
}

PlayerDeck & Player::getPlayerDeck()
{
	return player_deck_;
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

// </class Player> 



// <Class Deck>

Deck::Deck(const std::vector<Card>& deck)
	: deck_(deck) 
	, deck_it_(deck_.begin()) 
{}

Deck::Deck(const Deck & other)
	: deck_(other.deck_)
	, deck_it_(deck_.begin())
{}

Deck::Deck()
{
	std::vector<figures> figure_ = { NINE, TEN, JACK, QUEEN, KING, ACE };
	std::vector<suits> suit_ = { DIAMONDS, CLUBS, HEARTS, SPADES };
	for (auto& i : figure_) {
		for (auto& j : suit_) {
			deck_.emplace_back(i, j);
		}
	}
	deck_it_ = deck_.begin();
}

Deck::~Deck() {}

//clear player's decks and deal out new cards after shuffling the deck
void Deck::dealCards(players& players)
{
	shuffle();
	for (auto& i : players) {
		i.getPlayerDeck().clearDeck();
		for (int j = 0; j < MAX_CARDS; ++j) {
			i.getPlayerDeck().addCard(*deck_it_++);
		}
	}
}

json Deck::addBonusCards(Player & player)
{
	json msg;
	while (deck_it_ != deck_.end()) {
		json tmp = {
			{"figure", (*deck_it_).getFigure() },
			{"suit", (*deck_it_).getSuit()}
		};
		msg.push_back(tmp);
		player.getPlayerDeck().addCard(*deck_it_++);
	}
	deck_it_ = deck_.begin();
	return msg;
}

void Deck::shuffle()
{
	std::mt19937 engine(rd_());
	std::shuffle(deck_.begin(), deck_.end(), engine);
}

// </Class Deck>


// <Class PlayersCollection>

PlayersCollection::PlayersCollection()
{}

PlayersCollection::PlayersCollection(const PlayersCollection & other)
	: players_(other.players_)
{}

PlayersCollection::~PlayersCollection()
{}


players & PlayersCollection::getArray()
{
	return players_;
}

bool PlayersCollection::addPlayer(int player_id, std::string & nick)
{
	if (players_.size() < MAX_PLAYERS) {
		players_.push_back(Player(player_id, nick));
		return true;
	}
	return false;
}


size_t PlayersCollection::getNextPlayer(iterators it)
{
	if (players_.size() != MAX_PLAYERS) {
		throw std::out_of_range("Game has not started");
	}
	if (it == X) {
		throw std::logic_error("Non existing iterator called in getNextPlayer() method");
	}
	if (players_.back().getPlayerId() == it_[it]) {
		it_[it] = players_.front().getPlayerId();
		return it_[it];
	}
	for (size_t i = 0; i < players_.size(); ++i) {
		if (players_[i].getPlayerId() == it_[it]) {
			it_[it] = players_[i + 1].getPlayerId();
			return it_[it];
		}
	}
	return -1;
}

request_type PlayersCollection::getPlayerInfo() const
{
	request_type vec;
	for (auto& i : players_) {
		json tmp;
		tmp["id"] = i.getPlayerId();
		tmp["nick"] = i.getPlayersNick();
		vec.push_back(tmp);
	}
	return vec;
}

Player & PlayersCollection::getPlayer(iterators it, size_t player_id)
{
	if (players_.size() != MAX_PLAYERS) {
		throw std::out_of_range("Game has not started");
	}
	size_t index;
	if (it != X) {
		index = it_[it];
	}
	else {
		index = player_id;
	}
	for (auto& i : players_) {
		if (i.getPlayerId() == index)
			return i;
	}
	throw std::out_of_range("No player found based on given player id");
}

void PlayersCollection::setPlayer(iterators it, size_t player_id)
{
	if (players_.size() != MAX_PLAYERS) {
		throw std::out_of_range("Game has not started");
	}
	if (it == X) {
		throw std::logic_error("Argument \"X\" passed to setPlayer");
	}
	it_[it] = player_id;
}

void PlayersCollection::prepareGame()
{
	it_.resize(MAX_PLAYERS);
	it_[CURRENT] = players_[1].getPlayerId();
	it_[COMPULSORY] = players_[0].getPlayerId();
	it_[HIGHEST] = players_[0].getPlayerId();
	players_[0].getScoreClass().setClaim(100, false);
}

// </Class PlayerCollection>



// <Class Room>

Room::Room(int room_id, GameManager & man)
	: man_(man)
	, room_id_(room_id) 
	, stage_(ADDING) 
	, adder_(deck_, players_)
	, bidder_(deck_, players_)
	, dealer_(deck_, players_)
	, game_(deck_, players_)
	, score_(deck_, players_)
{}

Room::Room(const Room & other)
	: man_(other.man_)
	, room_id_(other.room_id_)
	, deck_(other.deck_)
    , players_(other.players_)
	, stage_(other.stage_)
	, adder_(other.adder_)
	, bidder_(other.bidder_)
	, dealer_(other.dealer_)
	, game_(other.game_)
	, score_(other.score_)
{}

Room::~Room()
{}

void Room::changeStage(stage new_stage)
{
	stage_ = new_stage;
}

bool Room::runGame(const json & msg)
{
	bool ret_val = false;
	request_type request, tmp;
	json feedback;
	std::vector<int> players_ids;
	if (parse(msg["action"]) == DISCONNECT || parse(msg["action"]) == LEAVE) {
		for (players_it it = players_.getArray().begin(); it != players_.getArray().end(); ++it) {
			if ((*it).getPlayerId() == msg["player"]) {
				players_.getArray().erase(it);
				score_.resetPlayersAtributes();
				for (auto i : players_.getArray()) {
					i.setReady(false);
				}
			}
		}
		feedback["action"] = "leave";
		feedback["player"] = msg["player"];
		for (auto& i : players_.getArray()) {
			feedback["who"].push_back(i.getPlayerId());
		}
		request.push_back(feedback);
		if (stage_ != ADDING || stage_ != ENDING) {
			feedback.erase("action");
			feedback["action"] = "end";
			request.push_back(feedback);
		}
		man_.pushMessage(request);
		return true;
	}
	if (parse(msg["action"]) == CHAT) {
		request.push_back(chatMessage(msg));
		man_.pushMessage(request);
		return true;
	}
	stage temp_stage = FAIL;
	switch (stage_) {
	case ADDING:
		switch (parse(msg["action"])) {
		case ADD:
			temp_stage = adder_.addPlayer(msg["player"], msg["data"]);
			if (temp_stage != FAIL) {
				ret_val = true;
				request_type player_info = players_.getPlayerInfo();
				feedback["who"] = msg["player"];
				feedback["action"] = "add";
				feedback["error"] = false;
				for (auto& i : player_info) {
					feedback["data"].push_back(i);
				}
				request.push_back(feedback);
				if (players_.getArray().size() > 1) {
					feedback.clear();
					feedback["action"] = "new_player";
					for (auto& i : player_info) {
						if (i["id"] != msg["player"]) {
							feedback["who"].push_back(i["id"]);
						}
						else {
							feedback["data"] = i;
						}
					}
					request.push_back(feedback);
				}
			}
			else {
				feedback["action"] = "add";
				feedback["error"] = true;
				feedback["who"] = msg["player"];
				request.push_back(feedback);
			}
			break;
		case READY:
			temp_stage = adder_.setPlayerReady(msg["player"], true);
			feedback["action"] = "ready";
			feedback["player"] = msg["player"];
			for (auto i : players_.getArray()) {
				if (i.getPlayerId() != msg["player"]) {
					feedback["who"].push_back(i.getPlayerId());
				}
			}
			request.push_back(feedback);
			if (temp_stage == ADDING) {
				break;
			}
			stage_ = temp_stage;
			players_.prepareGame();
			dealer_.dealCards();
			tmp = dealer_.createMessages();
			for (const auto& i : tmp) {
				request.push_back(i);
			}
			feedback.clear();
			feedback["data"] = 100;
			feedback["player"] = players_.getPlayer(COMPULSORY).getPlayerId();
			tmp = bidder_.createMessages(feedback, stage_, false);
			for (const auto& i : tmp) {
				request.push_back(i);
			}
			break;
		default: break;
		}
		break;
	case BIDDING:
		switch (parse(msg["action"])) {
		case BID: 
			temp_stage = bidder_.bid(msg["player"], msg["data"]);
			tmp = bidder_.createMessages(msg, temp_stage, false);
			for (auto& i : tmp) {
				request.push_back(i);
			}
			if (temp_stage == DEALING) {
				stage_ = DEALING;
				feedback["action"] = "stock";
				feedback["player"] = players_.getPlayer(HIGHEST).getPlayerId();
				feedback["data"] = deck_.addBonusCards(players_.getPlayer(HIGHEST));
				feedback.erase("who");
				for (const auto& i : players_.getArray()) {
					feedback["who"].push_back(i.getPlayerId());
				}
				request.push_back(feedback);
			}
			break;
		default: break;
		}
		break;
	case DEALING:
		switch (parse(msg["action"])) {
		case DEAL:
			for (const auto& i : msg["data"]) {
				dealer_.giveCardToPeer(i["player"], i["card"]);
			}
			feedback["action"] = "deal";
			for (auto& i : players_.getArray()) {
				if (i.getPlayerId() != players_.getPlayer(HIGHEST).getPlayerId()) {
					feedback["who"] = i.getPlayerId();
					for (auto& j : msg["data"]) {
						if (j["player"] == i.getPlayerId()) {
							Card tmp = players_.getPlayer(X, i.getPlayerId()).getPlayerDeck().getDeck().back();
							json k = {
								{"figure", tmp.getFigure()},
								{"suit", tmp.getSuit()}
							};
							feedback["data"].push_back(k);
						}
					}
					request.push_back(feedback);
					feedback.erase("data");
					feedback.erase("who");
				}
			}
			feedback.erase("action");
			feedback = bidder_.createMessages(msg, stage_, true)[0];
			request.push_back(feedback);
			break;
		case BID:
			bidder_.bid(msg["player"], msg["data"]);
			feedback["action"] = "start";
			feedback["data"] = players_.getPlayer(HIGHEST).getScoreClass().getClaim();
			feedback["player"] = players_.getPlayer(HIGHEST).getPlayerId();
			for (auto& i : players_.getArray()) {
				feedback["who"].push_back(i.getPlayerId());
			}
			request.push_back(feedback);
			stage_ = PLAYING;
		default: break;
		}
		break;
	case PLAYING:
		switch (parse(msg["action"])) {
		case PLAY:
			temp_stage = game_.manageTurn(msg["player"], msg["data"]);
			tmp = game_.createMessages(temp_stage);
			for (const auto& i : tmp) {
				request.push_back(i);
			}
			if (temp_stage != stage_) {
				stage_ = score_.sumUpScore();
				tmp = score_.createMessages(stage_);
				for (const auto& i : tmp) {
					request.push_back(i);
				}
			}
			if (stage_ == BIDDING) {
				dealer_.dealCards();
				tmp = dealer_.createMessages();
				for (const auto& i : tmp) {
					request.push_back(i);
				}
				feedback["data"] = 100;
				feedback["player"] = players_.getPlayer(COMPULSORY).getPlayerId();
				tmp = bidder_.createMessages(feedback, stage_, false);
				for (const auto& i : tmp) {
					request.push_back(i);
				}
			}
		}
		break;
	default: break;
	}
	 man_.pushMessage(request);
	 return ret_val;
}

int Room::parse(const std::string & str)
{
	json command = { { "add", ADD }, { "deal", DEAL }, { "bid", BID },
					{ "play", PLAY }, { "chat", CHAT }, { "ready", READY },
					{"leave", LEAVE}, { "disconnect", DISCONNECT },
	};
	return command[str];
}

bool Room::isEmpty()
{
	return players_.getArray().empty();
}

size_t Room::getRoomId() const
{
	return room_id_;
}

json Room::getPlayersInfo() const
{
	json players;
	players["id"] = room_id_;
	request_type info = players_.getPlayerInfo();
	for (auto& i : info) {
			players["nick"].push_back(i["nick"]);
	}
	return players;
}

json Room::chatMessage(const json & msg)
{
	json response = {
		{"action", "chat"},
		{"player", msg["player"]},
		{"data", msg["data"]},
	};
	for (auto& i : players_.getArray()) {
		if (i.getPlayerId() != msg["player"]) {
			response["who"].push_back(i.getPlayerId());
		}
	}
	return response;
}

// </Class Room>



// <Class Controller>
Controller::Controller(Deck & deck, PlayersCollection & players)
	: deck_(deck)
	, players_(players)
{}

Controller::~Controller()
{}

// </Class Controller>



// <Class Adder>

Adder::Adder(Deck & deck, PlayersCollection & players)
	: Controller(deck, players)
{}

Adder::~Adder()
{}

stage Adder::addPlayer(int player_id, std::string nick)
{
	if (!players_.addPlayer(player_id, nick)) {
		return FAIL;
	}
	return ADDING;
}

stage Adder::setPlayerReady(int player_id, bool isReady)
{
	players_.getPlayer(X, player_id).setReady(isReady);
	if (players_.getArray().size() == MAX_PLAYERS) {
		for (auto& i : players_.getArray()) {
			if (!i.getReady()) {
				return ADDING;
			}
		}
		return BIDDING;
	}
	return ADDING;
}

bool Adder::isFull() const
{
	return (players_.getArray().size() == MAX_PLAYERS);
}

// </Class Adder>



// <Class Bidder>

Bidder::Bidder(Deck & deck, PlayersCollection & players)  
	: Controller(deck, players)
{}

Bidder::~Bidder()
{}

stage Bidder::bid(int player_id, int new_amount)
{
	if (players_.getPlayer(CURRENT).getPlayerId() != player_id) {
		throw std::logic_error("Player trying to bid not at his turn");
	}
	if (players_.getPlayer(HIGHEST).getScoreClass().getClaim() > new_amount && new_amount != -1) {
		throw std::logic_error("Player bids less than current highest bid");
	}
	if (players_.getPlayer(CURRENT).getPlayerId() == players_.getPlayer(HIGHEST).getPlayerId()) {
		players_.getPlayer(CURRENT).getScoreClass().setClaim(new_amount, true);
		return DEALING;
	}
	players_.getPlayer(CURRENT).getScoreClass().setClaim(new_amount, false);
	if (new_amount != -1) {
		players_.setPlayer(HIGHEST, player_id);
	}
	players_.getNextPlayer(CURRENT);
	while (players_.getPlayer(CURRENT).getScoreClass().getClaim() == -1) {
		players_.getNextPlayer(CURRENT);
	}
	if (players_.getPlayer(CURRENT).getPlayerId() ==
		players_.getPlayer(HIGHEST).getPlayerId()) {
		return DEALING;
	}
	return BIDDING;
}

void Bidder::giveAddCards()
{
	deck_.addBonusCards(players_.getPlayer(HIGHEST));
}

request_type Bidder::createMessages(const json & msg, stage stage_, bool isLastBid)
{
	request_type request;
	json feedback;
	feedback["action"] = "bid";
	if (stage_ == DEALING && isLastBid) {
		feedback["player"] = players_.getPlayer(HIGHEST).getPlayerId();
		feedback["data"] = {
			{ "value", players_.getPlayer(HIGHEST).getScoreClass().getClaim() },
			{ "id", feedback["player"] },
			{"min", players_.getPlayer(HIGHEST).getScoreClass().getClaim() },
			{"max", players_.getPlayer(HIGHEST).getPlayerDeck().getMaxValue(true)}
		};
		feedback["who"] = feedback["player"];
		request.push_back(feedback);
		return request;
	}
	for (auto i : players_.getArray()) {
		if (i.getPlayerId() != players_.getPlayer(CURRENT).getPlayerId() || stage_ == DEALING) {
			feedback["who"].push_back(i.getPlayerId());
		}
	}
	if (stage_ == BIDDING) {
		feedback["player"] = players_.getPlayer(CURRENT).getPlayerId();
	}
	else {
		feedback["player"] = -1;
	}
	feedback["data"] = {
		{ "value", msg["data"] },
		{ "id", msg["player"] }
	};
	request.push_back(feedback);
	if (stage_ == BIDDING) {
		feedback.erase("who");
		feedback["who"] = players_.getPlayer(CURRENT).getPlayerId();
		feedback["data"]["min"] = players_.getPlayer(HIGHEST).getScoreClass().getClaim() + MIN_RAISE;
		feedback["data"]["max"] = players_.getPlayer(CURRENT).getPlayerDeck().getMaxValue(false);
		request.push_back(feedback);
	}
	return request;
}

// </Class Bidder>



// <Class Dealer>

Dealer::Dealer(Deck & deck, PlayersCollection & players) 
	: user_id_(-1) 
	, counter(0)
	, Controller(deck, players)
{}

Dealer::~Dealer()
{}

stage Dealer::giveCardToPeer(int player_id, std::size_t card_number)
{
	if ( player_id == (players_.getPlayer(HIGHEST)).getPlayerId() || (player_id == user_id_ &&
		counter == 1)) {
		throw std::logic_error("Giving card to the wrong player");
	}	
	Card tmp = players_.getPlayer(HIGHEST).getPlayerDeck().playCard(card_number);
	tmp.setIsUsed(false);
	players_.getPlayer(X, player_id).getPlayerDeck().addCard(tmp);
	user_id_ = players_.getPlayer(X, player_id).getPlayerId();
	if (++counter == TWO_CARDS) {
		return PLAYING;
	}
	return DEALING;
}

void Dealer::dealCards()
{
	deck_.shuffle();
	deck_.dealCards(players_.getArray());
}

request_type Dealer::createMessages()
{
	json feedback;
	request_type request;
	feedback["action"] = "deal";
	for (auto i : players_.getArray()) {
		feedback["who"] = i.getPlayerId();
		for (auto j : i.getPlayerDeck().getDeck()) {
			json tmp = {
				{ "figure", j.getFigure() }
				,{ "suit", j.getSuit() }
			};
			feedback["data"].push_back(tmp);
		}
		request.push_back(feedback);
		feedback.erase("who");
		feedback.erase("data");
	}
	return request;
}

void Dealer::reset()
{
	user_id_ = -1;
	counter = 0;
}

// </Class Dealer>



// <Class Game>

Game::Game(Deck & deck, PlayersCollection & players)
	: turn_counter_(0) 
	, super_suit_(NONE) 
	, Controller(deck, players)
	, is_marriage_(false)
{}

Game::~Game()
{}

const Card & Game::playTurn(int player, std::size_t card)
{
	return players_.getPlayer(X, player).getPlayerDeck().playCard(card);
}

stage Game::manageTurn(int player, int card)
{
	if (players_.getPlayer(CURRENT).getPlayerId() != player) {
		throw std::logic_error("Not player's turn to play a card");
	}
	vec_.emplace_back(std::make_pair(player, playTurn(player, card) ));
	if (vec_.size() == 1 && vec_[0].second.getFigure() == QUEEN 
		|| vec_[0].second.getFigure() == KING) {
			is_marriage_ = players_.getPlayer(CURRENT).getPlayerDeck().doesHavePair(vec_[0].second.getSuit());
			if (is_marriage_) {
				super_suit_ = vec_[0].second.getSuit();
			}
	}
	else {
		is_marriage_ = false;
	}
	players_.getNextPlayer(CURRENT);
	if (vec_.size() == MAX_PLAYERS) {
		players_.setPlayer(CURRENT, compareCardsAndPassToWinner());
		if (++turn_counter_ == MAX_TURNS) {
			reset(); 
			return ENDING;
		}
	}
	return PLAYING;
}

request_type Game::createMessages(const stage stage_)
{
	request_type request;
	json feedback = {
		{"action", "play"},
		{ "player", players_.getPlayer(CURRENT).getPlayerId() }
	};
	if (stage_ == SUMMING_UP) {
		feedback.erase("player");
		feedback["player"] = -1;
	}
	json tmpj = {
		{ "figure", vec_.back().second.getFigure() },
		{ "suit", vec_.back().second.getSuit() },
		{"player", vec_.back().first},
		{"marriage", is_marriage_}
	};
	if (vec_.size() == MAX_PLAYERS) {
		vec_.clear();
	}
	std::vector<Card> tmp;
	for (const auto& i : vec_) {
		tmp.push_back(i.second);
	}
	for (auto& i : players_.getArray()) {
		feedback["who"] = i.getPlayerId();
		feedback["data"]["prev"] = tmpj;
		if (feedback["who"] == feedback["player"]) {
			feedback["data"]["available"] = i.getPlayerDeck().getAllValidCards(tmp, super_suit_);
			request.push_back(feedback);
			feedback.erase("data");
		}
		else {
			request.push_back(feedback);
		}
		feedback.erase("who");
	}
	return request;
}

int Game::setSuperiorSuit()
{
	int score = 0;
	if (vec_[0].second.getFigure() == KING || vec_[0].second.getFigure() == QUEEN) {
		if (players_.getPlayer(X, current_starting_player_).getPlayerDeck().doesHavePair(
			vec_[0].second.getSuit())) {
			super_suit_ = vec_[0].second.getSuit();
			switch (super_suit_) {
			case SPADES: score = SPADES; break;
			case CLUBS: score = CLUBS; break;
			case DIAMONDS: score = DIAMONDS; break;
			case HEARTS: score = HEARTS; break;
			default: break;
			}
		}
	}
	if (score != 0) {
		is_marriage_ = true;
	}
	return score;
}

int Game::compareCardsAndPassToWinner()
{
	figures superior_suit_figure = NOT_A_FIGURE;
	figures ordinary_figure = NOT_A_FIGURE;
	suits current_suit = NONE;
	current_starting_player_ = vec_[0].first;
	int score = 0;
	if (is_marriage_) {
		score = super_suit_;
	}
	if (super_suit_ != NONE) {
		for (auto& i : vec_) {
			if (i.second.getSuit() == super_suit_) {
				if (superior_suit_figure < i.second.getFigure()) {
					superior_suit_figure = i.second.getFigure();
					current_starting_player_ = i.first;
				}
			}
		}
	}
	if (superior_suit_figure == NOT_A_FIGURE) {
		for (auto& i : vec_) {
			if (i.first == current_starting_player_) {
				superior_suit_figure = i.second.getFigure();
				current_suit = i.second.getSuit();
			}
		}
		for (auto& i : vec_) {
			if (i.second.getSuit() == current_suit && 
				i.second.getFigure() > superior_suit_figure) {
				superior_suit_figure = i.second.getFigure();
				current_starting_player_ = i.first;
			}
		}
	}
	for (auto& i : vec_) {
		score += i.second.getFigure();
	}
	players_.getPlayer(X, current_starting_player_).getScoreClass().addToTurnScore(score);
	return current_starting_player_;
}

void Game::setStartingPlayer(int new_starting_player)
{
	current_starting_player_ = new_starting_player;
}

void Game::reset()
{
	turn_counter_ = 0;
	current_starting_player_ = 0;
	super_suit_ = NONE;
}

// </Class Game>



// <Class SumScore>

SumScore::SumScore(Deck & deck, PlayersCollection & players)
	: Controller(deck, players)
{}

SumScore::~SumScore()
{}

stage SumScore::sumUpScore()
{
	for (auto& i : players_.getArray()) {
		if (i.getPlayerId() == players_.getPlayer(HIGHEST).getPlayerId()) {
			if (i.getScoreClass().getClaim() > i.getScoreClass().getTurnScore()) {
				i.getScoreClass().addToTurnScore(-1 * (i.getScoreClass().getTurnScore() + i.getScoreClass().getClaim()));
				i.getScoreClass().addScore((-1) * i.getScoreClass().getClaim());
			}
			else {
				i.getScoreClass().addScore(i.getScoreClass().getClaim());
			}
		}
		else {
			if (i.getScoreClass().getScore() < 800) {
				if (i.getScoreClass().getTurnScore() % 10) {
					double rounded = static_cast<double>(i.getScoreClass().getTurnScore());
					rounded /= 10;
					i.getScoreClass().addToTurnScore((-1) * i.getScoreClass().getTurnScore());
					i.getScoreClass().addToTurnScore(round(static_cast<int>(rounded))*10);
				}
				i.getScoreClass().addScore(i.getScoreClass().getTurnScore());
			}
		}
		if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
			for (auto& i : players_.getArray()) {
				i.setReady(false);
			}
			return ENDING;
		}
	}
	return BIDDING;
}

void SumScore::resetPlayersAtributes()
{
	for (auto& i : players_.getArray()) {
		i.getScoreClass().clearTurnScore();
		i.getScoreClass().resetClaim();
		i.getPlayerDeck().clearDeck();
	}
	size_t player_id = players_.getNextPlayer(COMPULSORY);
	players_.setPlayer(CURRENT, player_id);
	players_.setPlayer(HIGHEST, player_id);
	players_.getNextPlayer(CURRENT);
	players_.getPlayer(COMPULSORY).getScoreClass().setClaim(100, false);
}

bool SumScore::isFinished() const
{
	for (auto& i : players_.getArray()) {
		if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
			return true;
		}
	}
	return false;
}

request_type SumScore::createMessages(stage stages_)
{
	json message;
	request_type request;
	message["action"] = "score";
	for (auto& i : players_.getArray()) {
		message["who"].push_back(i.getPlayerId());
		json tmp = {
			{"player", i.getPlayerId() }, 
			{"score", i.getScoreClass().getTurnScore() }
		};
		message["data"].push_back(tmp);
	}
	request.push_back(message);
	resetPlayersAtributes();
	if (stages_ == ENDING) {
		json message = {
			{"action", "end"}
		};
		for (auto& i : players_.getArray()) {
			if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
				message["data"] = i.getScoreClass().getScore();
				message["player"] = i.getPlayerId();
			}
			message["who"].push_back(i.getPlayerId());
		}
		request.push_back(message);
	}
	return request;
}

// </Class SumScore>



// <Class AddManager>
AddManager::AddManager(std::vector<PRoom> & active_games)
	: active_games_(active_games)
{}


AddManager::~AddManager()
{}

req AddManager::addPlayer(const json & msg)
{
	return req();
}


// </Class AddManager>



// <Class GameManager>

GameManager::GameManager() 
	: room_counter_(0)
{}

GameManager::~GameManager()
{}

req GameManager::doWork(std::size_t player_id, const std::string & message)
{
	server_response_.clear();
	feedback_.clear();
	try {
		json msg = json::parse(message.begin(), message.end());
		msg["player"] = player_id;
		if (msg["action"] == "add") {
			addPlayer(msg);
			return server_response_;
		}
		if (msg["action"] == "show") {
			returnExistingRooms(msg);
			return server_response_;
		}
		msg["id"] = findGameId(player_id);
		active_games_[msg["id"]]->runGame(msg);
		attachClientIdsToMessage();
		return server_response_;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return server_response_;
}

void GameManager::returnExistingRooms(const json & msg)
{
	json resp = {
		  {"action", "show"}
		, {"who", msg["player"]}
	};
	for (auto& i : active_games_) {
		if (!i->isEmpty()) {
			resp["data"].push_back(i->getPlayersInfo());
		}
	}
	feedback_.push_back(resp);
	attachClientIdsToMessage();
}

void GameManager::attachClientIdsToMessage()
{
	for (auto& i : feedback_) {
		std::vector<int> v;
		for (auto& a : i["who"]) {
			v.push_back(static_cast<int>(a));
		}
		i.erase("who");
		server_response_.emplace_back(std::make_pair(i.dump(), v));
	}
}

void GameManager::pushMessage(const request_type & msg)
{
	feedback_ = msg;
}

int GameManager::findGameId(size_t player_id) const
{
	for (size_t i = 0; i < players_.size(); ++i) {
		for (auto& j : players_[i]) {
			if (j == player_id) {
				return i;
			}
		}
	}
	return -1;
}

void GameManager::addPlayer(json & msg)
{
	if (msg["id"] == -1) {
		for (size_t i = 0; i < active_games_.size(); ++i) {
			if (active_games_[i]->isEmpty()) {
				active_games_[i]->runGame(msg);
				players_[i].push_back(msg["player"]);
				attachClientIdsToMessage();
				return;
			}
		}
		active_games_.emplace_back(std::make_shared<Room>(room_counter_++, *this));
		if (active_games_.back()->runGame(msg)) {
			players_.emplace_back();
			players_.back().push_back(msg["player"]);
		}
		attachClientIdsToMessage();
		return;
	}
	if (static_cast<size_t>(msg["id"]) < active_games_.size()) {
		if (active_games_[msg["id"]]->runGame(msg)) {
			players_[msg["id"]].push_back(msg["player"]);
		}
		attachClientIdsToMessage();
	}
	return;
	 //Tutaj mo�e by� r�nie w zale�no�ci od tego
	 //jak b�dzie na poziomie Roomu obs�ugiwany brak dodania
}

// </Class GameManager>

