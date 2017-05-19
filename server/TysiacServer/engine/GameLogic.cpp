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

bool Card::operator==(const Card & other)
{
	return (suit_ == other.suit_ && figure_ == other.figure_);
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

const Card & Card::isBigger(const Card & other, suits suit)
{
	if (suit_ == other.suit_) {
		if (figure_ > other.figure_) {
			return *this;
		}
		else {
			return other;
		}
	}
	else if (other.suit_ == suit) {
		return other;
	}
	return *this;
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

void Score::reset(bool isFinal)
{
	temp_score_ = 0;
	claim_ = 0;
	if (isFinal) {
		score_ = 0;
	}
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

void Score::roundScore()
{
	if (claim_ != -1) {
		if (temp_score_ >= claim_) {
			temp_score_ = claim_;
		}
		else {
			temp_score_ = (-1)*claim_;
		}
	}
	else if (score_ >= NO_TAX_CAP) {
		temp_score_ = 0;
	}
	else {
		temp_score_ = round(temp_score_);
	}
}

int Score::round(int number) const
{
	int temp = number % 10;
	if (temp) {
		if (temp >=5) {
			number += (10 - temp);
		}
		else {
			number -= temp;
		}
	}
	return number;
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
	for (size_t i = 0; i < players_.size(); ++i) {
		if (players_[i].getPlayerId() == it_[it]) {
			it_[it] = players_[(i + 1) % MAX_PLAYERS].getPlayerId();
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

void PlayersCollection::prepareGame(bool isFirst)
{
	int i = isFirst ? 0 : (it_[COMPULSORY] + 1) % MAX_PLAYERS;
	it_.resize(MAX_PLAYERS);
	it_[CURRENT] = players_[(i+1) % MAX_PLAYERS].getPlayerId();
	it_[COMPULSORY] = players_[i].getPlayerId();
	it_[HIGHEST] = players_[i].getPlayerId();
	players_[i].getScoreClass().setClaim(100, false);

}

void PlayersCollection::resetPlayerAttributes(bool isFinal)
{
	for (auto& i : players_) {
		i.getScoreClass().reset(isFinal);
		i.getPlayerDeck().clearDeck();
		if (isFinal) {
			i.setReady(false);
		}
	}
	if (isFinal) {
		it_.clear();
	}
}

// </Class PlayerCollection>



// <Class Room>

Room::Room(int room_id, GameManager & man)
	: man_(man)
	, room_id_(room_id)
	, stage_(ADDING)
{
	employees_.push_back(new LeaveBuster(deck_, players_));
	employees_.push_back(new ChatBox(deck_, players_));
	employees_.push_back(new Adder(deck_, players_));
	employees_.push_back(new Bidder(deck_, players_));
	employees_.push_back(new Dealer(deck_, players_));
	employees_.push_back(new Game(deck_, players_));
}

Room::Room(const Room & other)
	: man_(other.man_)
	, employees_(other.employees_.begin(), other.employees_.end())
	, room_id_(other.room_id_)
	, deck_(other.deck_)
    , players_(other.players_)
	, stage_(other.stage_)
{}

Room::~Room()
{
	for (auto i : employees_) {
		delete i;
	}
}

void Room::changeStage(stage new_stage)
{
	stage_ = new_stage;
}

bool Room::runGame(const json & msg)
{
	request_type request, tmp;
	stage temp_stage;
	temp_stage = employees_[parse(msg["action"])]->changeModel(msg, stage_);
	request = employees_[parse(msg["action"])]->createMessages(msg, temp_stage);
	if (temp_stage == LEAVING) {
		stage_ = ADDING;
	}
	else {
		stage_ = temp_stage;
	}
	if (temp_stage == SUMMING_UP) {
		employees_[BID]->changeModel(msg, SUMMING_UP);
		tmp = employees_[BID]->createMessages(msg, SUMMING_UP);
		for (const auto & i : tmp) {
			request.push_back(i);
		}
		stage_ = BIDDING;
	}
	man_.pushMessage(request);
	if (temp_stage != FAIL) {
		return true;
	}
	return false;
	
}

int Room::parse(const std::string & str)
{
	json command = { { "add", ADD }, { "deal", DEAL }, { "bid", BID },
					{ "play", PLAY }, { "chat", CHAT }, { "ready", BID },
					{"leave", LEAVE}, { "disconnect", LEAVE }
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


// </Class Room>



 // <Class Controller>

Controller::Controller(Deck & deck, PlayersCollection & players)
	: deck_(deck)
	, players_(players)
{}

Controller::~Controller()
{}

// </Class Controller>



// <Class LeaveBuster>

LeaveBuster::LeaveBuster(Deck & deck, PlayersCollection & players) 
	: Controller(deck, players)
{}

LeaveBuster::~LeaveBuster()
{}

stage LeaveBuster::changeModel(const json & msg, const stage stage_)
{
	for (auto i = players_.getArray().begin(); i != players_.getArray().end(); ++i) {
		if ((*i).getPlayerId() == msg["player"]) {
			players_.getArray().erase(i);
			break;
		}
	}
	players_.resetPlayerAttributes(true);
	if (stage_ != ADDING && stage_ != ENDING) {
		return LEAVING;
	}
	else return ADDING;
}

request_type LeaveBuster::createMessages(const json & msg, const stage stage_)
{
	json feedback;
	request_type request;
	feedback["action"] = "leave";
	feedback["data"] = msg["player"];
	for (auto& i : players_.getArray()) {
		feedback["who"].push_back(i.getPlayerId());
	}
	request.push_back(feedback);
	if (stage_ != ADDING && stage_ != ENDING) {
		feedback.erase("action");
		feedback.erase("data");
		feedback["player"] = -1;
		feedback["action"] = "end";
		request.push_back(feedback);
	}
	return request;
}

 // </Class LeaveBuster>



// <Class ChatBox>

ChatBox::ChatBox(Deck & deck, PlayersCollection & players)
	: Controller(deck, players)
{}

ChatBox::~ChatBox()
{}

stage ChatBox::changeModel(const json & msg, const stage stage_)
{
	return stage_;
}

request_type ChatBox::createMessages(const json & msg, const stage stage_)
{
	request_type request;
	json response = {
		{ "action", "chat" },
		{ "player", msg["player"] },
		{ "data", msg["data"] },
	};
	for (auto& i : players_.getArray()) {
		if (i.getPlayerId() != msg["player"]) {
			response["who"].push_back(i.getPlayerId());
		}
	}
	request.push_back(response);
	return request;
}

// </Class ChatBox>



// <Class Adder>

Adder::Adder(Deck & deck, PlayersCollection & players)
	: Controller(deck, players)
{}

Adder::~Adder()
{}

stage Adder::changeModel(const json & msg, const stage stage_)
{
	std::string tmp = msg["data"];
	if (!players_.addPlayer(msg["player"], tmp)) {
		return FAIL;
	}
	return ADDING;
}

request_type Adder::createMessages(const json & msg, const stage stage_)
{
	json feedback;
	request_type request;
	request = informOtherPlayers(msg, stage_);
	request.push_back(acceptNewPlayer(msg, stage_));
	return request;
}

bool Adder::isFull() const
{
	return (players_.getArray().size() == MAX_PLAYERS);
}

json Adder::acceptNewPlayer(const json & msg, stage stage_)
{
	json feedback;
	request_type request, player_info = players_.getPlayerInfo();
	feedback["who"] = msg["player"];
	feedback["action"] = "add";
	if (stage_ != FAIL) {
		feedback["error"] = false;
		for (auto& i : player_info) {
			feedback["data"].push_back(i);
		}
	}
	else {
		feedback["error"] = true;
	}
	return feedback;
}

request_type Adder::informOtherPlayers(const json & msg, stage stage_)
{
	request_type request, player_info = players_.getPlayerInfo();
	json feedback;
	if (stage_ != FAIL) {
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
	return request;
}

// </Class Adder>



// <Class Starter>

Starter::Starter(Deck & deck, PlayersCollection & players)
	: Controller(deck, players)
	, is_full_(false)
{}

Starter::~Starter()
{}

stage Starter::changeModel(const json & msg, const stage stage_)
{
	if (stage_ == SUMMING_UP) {
		prepareToStart(stage_);
		return BIDDING;
	}
	players_.getPlayer(X, msg["player"]).setReady(true);
	if (isReadyToStart()) {
		prepareToStart(stage_);
		return BIDDING;
	}
	return ADDING;
}

request_type Starter::createMessages(const json & msg, const stage stage_)
{
	json feedback;
	request_type request;
	feedback["action"] = "ready";
	feedback["player"] = msg["player"];
	for (auto i : players_.getArray()) {
		if (i.getPlayerId() != msg["player"]) {
			feedback["who"].push_back(i.getPlayerId());
		}
	}
	request.push_back(feedback);
	return request;
}

json Starter::createStartMessage(const json & msg) const
{
	json feedback;
	feedback["action"] = "start";
	feedback["data"] = players_.getPlayer(HIGHEST).getScoreClass().getClaim();
	feedback["player"] = players_.getPlayer(HIGHEST).getPlayerId();
	for (auto& i : players_.getArray()) {
		feedback["who"].push_back(i.getPlayerId());
	}
	return feedback;
}

bool Starter::getIsFull() const
{
	return is_full_;
}

void Starter::setIsFull(bool is_full)
{
	is_full_ = is_full;
}

bool Starter::isReadyToStart() const
{
	if (players_.getArray().size() != MAX_PLAYERS) {
		return false;
	}
	for (auto& i : players_.getArray()) {
		if (!i.getReady()) {
			return false;
		}
	}
	return true;
}

void Starter::prepareToStart(stage stage_)
{
	is_full_ = false;
	if (stage_ == SUMMING_UP) {
		players_.prepareGame(false);
	}
	else {
		players_.prepareGame(true);
	}
	deck_.shuffle();
	deck_.dealCards(players_.getArray());
}

// </Class Starter>



// <Class Bidder>

Bidder::Bidder(Deck & deck, PlayersCollection & players)  
	: Controller(deck, players)
	, starter_(deck, players)
{}

Bidder::~Bidder()
{}

stage Bidder::bid(const json & msg, const stage stage_)
{
	if (players_.getPlayer(CURRENT).getPlayerId() != msg["player"]) {
		throw std::logic_error("Player trying to bid not at his turn");
	}
	if (players_.getPlayer(HIGHEST).getScoreClass().getClaim() > static_cast<int>(msg["data"])
		&& msg["data"] != -1) {
		throw std::logic_error("Player bids less than current highest bid");
	}
	if (players_.getPlayer(CURRENT).getPlayerId() == players_.getPlayer(HIGHEST).getPlayerId()) {
		players_.getPlayer(CURRENT).getScoreClass().setClaim(msg["data"], true);
		return PLAYING;
	}
	players_.getPlayer(CURRENT).getScoreClass().setClaim(msg["data"], false);
	if (msg["data"] != -1) {
		players_.setPlayer(HIGHEST, msg["player"]);
	}
	players_.getNextPlayer(CURRENT);
	while (players_.getPlayer(CURRENT).getScoreClass().getClaim() == -1) {
		players_.getNextPlayer(CURRENT);
	}
	if (players_.getPlayer(CURRENT).getPlayerId() ==
		players_.getPlayer(HIGHEST).getPlayerId()) {
		additional_cards_ = deck_.addBonusCards(players_.getPlayer(HIGHEST));
		return DEALING;
	}
	return BIDDING;
}
stage Bidder::changeModel(const json & msg, const stage stage_)
{
	if (stage_ == ADDING || stage_ == SUMMING_UP) {
		return starter_.changeModel(msg, stage_);
	}
	else {
		return bid(msg, stage_);
	}
}

request_type Bidder::createMessages(const json & msg, stage stage_)
{
	request_type request, tmp;
	request = createStarterMessages(msg, stage_);
	if (request.empty()) {
		request = createBid(msg, stage_);
	}
	return request;
}

request_type Bidder::createSpecialInfo(const json & msg) const
{
	request_type request;
	json feedback;
	feedback["action"] = "bid";
	for (auto i : players_.getArray()) {
		feedback["who"].push_back(i.getPlayerId());
	}
	feedback["player"] = -1;
	feedback["data"] = {
		{ "value", msg["data"] },
		{ "id", msg["player"] }
	};
	request.push_back(feedback);
	return request;
}

request_type Bidder::createUpdateInfo(const json & msg) const
{
	json feedback;
	request_type request;
	for (auto i : players_.getArray()) {
		if (i.getPlayerId() != players_.getPlayer(CURRENT).getPlayerId()) {
			feedback["who"].push_back(i.getPlayerId());
		}
	}
	feedback["action"] = "bid";
	feedback["player"] = players_.getPlayer(CURRENT).getPlayerId();
	feedback["data"] = {
		{ "value", msg["data"] },
		{ "id", msg["player"] }
	};
	request.push_back(feedback);
	feedback.erase("who");
	feedback["who"] = players_.getPlayer(CURRENT).getPlayerId();
	feedback["data"]["min"] = players_.getPlayer(HIGHEST).getScoreClass().getClaim() + MIN_RAISE;
	feedback["data"]["max"] = players_.getPlayer(CURRENT).getPlayerDeck().getMaxValue(false);
	request.push_back(feedback);
	return request;
}

json Bidder::createStock(const json & msg) const
{
	json feedback;
	feedback["action"] = "stock";
	feedback["player"] = players_.getPlayer(HIGHEST).getPlayerId();
	feedback["data"] = additional_cards_;
	for (const auto& i : players_.getArray()) {
		feedback["who"].push_back(i.getPlayerId());
	}
	additional_cards_.clear();
	return feedback;
}

request_type Bidder::createBid(const json & msg, stage stage_) const
{
	request_type request;
	if (stage_ == BIDDING || stage_ == SUMMING_UP) {
		request = createUpdateInfo(msg);
	}
	else if (stage_ == PLAYING || stage_ == DEALING ) {
		request = createSpecialInfo(msg);
		if (stage_ == PLAYING) {
			request.push_back(starter_.createStartMessage(msg));
		}
		else {
			request.push_back(createStock(msg));
		}
	}
	return request;
}

request_type Bidder::firstBid(stage stage_)
{
		json feedback;
		request_type request;
		feedback["data"] = 100;
		feedback["player"] = players_.getPlayer(COMPULSORY).getPlayerId();
		request = createBid(feedback, stage_);
		return request;
}

request_type Bidder::createCardDealingMessages() const
{
	json feedback;
	request_type request;
	feedback["action"] = "deal";
	for (auto i : players_.getArray()) {
		feedback["who"] = i.getPlayerId();
		for (auto& j : i.getPlayerDeck().getDeck()) {
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

request_type Bidder::createStarterMessages(const json & msg, stage stage_)
{
	request_type tmp, request;
	if (!starter_.getIsFull()) {
		if (stage_ == BIDDING) {
			request = starter_.createMessages(msg, stage_);
		}
		if (stage_ == BIDDING || stage_ == SUMMING_UP) {
			tmp = createCardDealingMessages();
			for (const auto & i : tmp) {
				request.push_back(i);
			}
			tmp = firstBid(stage_);
			for (const auto & i : tmp) {
				request.push_back(i);
			}
			starter_.setIsFull(true);
		}
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

stage Dealer::changeModel(const json & msg, const stage stage_)
{
	for (const auto& i : msg["data"]) {
		giveCardToPeer(i["player"], i["card"]);
	}
	return DEALING;
}

request_type Dealer::createMessages(const json & msg, stage stage_)
{
	request_type request = createMessage(msg);
	request.push_back(createFinalBidMessage());
	return request;
}


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

request_type Dealer::createMessage(const json & msg)
{
	request_type request;
	for (const auto & i : msg["data"]) {
		json feedback = {
			{"action", "deal"},
			{"who", i["player"]}
		};
		Card tmp = players_.getPlayer(X, i["player"]).getPlayerDeck().getDeck().back();
		json tmp1 = {
			{"figure", tmp.getFigure()},
			{"suit", tmp.getSuit()}
		};
		feedback["data"].push_back(tmp1);
		request.push_back(feedback);
	}
	return request;
}

void Dealer::reset()
{
	user_id_ = -1;
	counter = 0;
}

json Dealer::createFinalBidMessage()
{
	json feedback;
	feedback["action"] = "bid";
	feedback["player"] = players_.getPlayer(HIGHEST).getPlayerId();
	feedback["data"] = {
		{ "value", players_.getPlayer(HIGHEST).getScoreClass().getClaim() },
		{ "id", feedback["player"] },
		{ "min", players_.getPlayer(HIGHEST).getScoreClass().getClaim() },
		{ "max", players_.getPlayer(HIGHEST).getPlayerDeck().getMaxValue(true) }
	};
	feedback["who"] = feedback["player"];
	return feedback;
}

// </Class Dealer>



// <Class Game>

Game::Game(Deck & deck, PlayersCollection & players)
	: score_(deck, players)
	, turn_counter_(0) 
	, super_suit_(NONE) 
	, Controller(deck, players)
	, is_marriage_(false)
{}

Game::~Game()
{}

stage Game::changeModel(const json & msg, const stage stage_)
{
	if (manageTurn(msg["player"], msg["data"]) == SUMMING_UP) {
		return score_.sumUpScore();
	}
	return PLAYING;
}

request_type Game::createMessages(const json & msg, stage stage_)
{
	request_type request = createMessage(stage_);
	if (stage_ == SUMMING_UP || stage_ == ENDING) {
		request_type tmp = score_.createMessages(msg, stage_);
		for (const auto & i : tmp) {
			request.push_back(i);
		}
	}
	return request;
}

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
	setSuperiorSuit();
	players_.getNextPlayer(CURRENT);
	if (vec_.size() == MAX_PLAYERS) {
		players_.setPlayer(CURRENT, compareCardsAndPassToWinner());
		if (++turn_counter_ == MAX_TURNS) {
			reset(); 
			return SUMMING_UP;
		}
	}
	return PLAYING;
}

request_type Game::createMessage(const stage stage_)
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

void Game::setSuperiorSuit()
{
	int score = 0;
	if (vec_.size() == 1 && vec_[0].second.getFigure() == KING || vec_[0].second.getFigure() == QUEEN) {
		if (players_.getPlayer(X, vec_[0].first).getPlayerDeck().doesHavePair(
			vec_[0].second.getSuit())) {
			super_suit_ = vec_[0].second.getSuit();
			players_.getPlayer(X, vec_[0].first).getScoreClass().addToTurnScore(super_suit_);
			is_marriage_ = true;
		}
	}
	else {
		is_marriage_ = false;
	}
}

int Game::compareCardsAndPassToWinner()
{
	Card tmp = vec_[0].second.isBigger(vec_[1].second.isBigger(vec_[2].second, super_suit_), super_suit_);
	for (const auto& i : vec_) {
		if (tmp == i.second) {
			current_starting_player_ = i.first;
		}
	}
	for (auto& i : vec_) {
		players_.getPlayer(X, current_starting_player_).getScoreClass().addToTurnScore(i.second.getFigure());
	}
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
		i.getScoreClass().roundScore();
		i.getScoreClass().addScore(i.getScoreClass().getTurnScore());
		if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
			for (auto& i : players_.getArray()) {
				i.setReady(false);
			}
			return ENDING;
		}
	}
	return SUMMING_UP;
}

stage SumScore::changeModel(const json & msg, const stage stage_)
{
	return sumUpScore();
}

request_type SumScore::createMessages(const json & msg, stage stage_)
{
	return createMessage(stage_);
}

void SumScore::resetPlayerAttributes(bool isFinal)
{
	players_.resetPlayerAttributes(isFinal);
	if (isFinal) {
		return;
	}
	players_.prepareGame(false);
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

request_type SumScore::createMessage(stage stages_)
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
	resetPlayerAttributes(false);
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
	json msg = json::parse(message.begin(), message.end());
	msg["player"] = player_id;
	try {
		if (!runGame(msg)) {
			active_games_[findGameId(player_id)]->runGame(msg);
			removeIfLeaveCalled(msg, player_id);
			attachClientIdsToMessage();
		}
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return server_response_;
}

void GameManager::removeIfLeaveCalled(const json& msg, int player_id)
{
	if (msg["action"] != "leave") {
		return;
	}
	for (auto & i : players_) {
		for (auto it = i.begin(); it != i.end(); ++it) {
			if (*it == player_id) {
				i.erase(it);
				return;
			}
		}
	}
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

void GameManager::addPlayer(const json & msg)
{
	if (msg["id"] == -1) {
		if (!useEmptyRoom(msg)) {
			createNewRoom(msg);
		}
		return;
	}
	if (static_cast<size_t>(msg["id"]) < active_games_.size()) {
		if (active_games_[msg["id"]]->runGame(msg)) {
			players_[msg["id"]].push_back(msg["player"]);
		}
		attachClientIdsToMessage();
	}
}

bool GameManager::runGame(const json & msg)
{
	if (msg["action"] == "add") {
		addPlayer(msg);
		return true;
	}
	if (msg["action"] == "show") {
		returnExistingRooms(msg);
		return true;
	}
	return false;
}

void GameManager::createNewRoom(const json & msg)
{
	active_games_.emplace_back(std::make_shared<Room>(room_counter_++, *this));
	if (active_games_.back()->runGame(msg)) {
		players_.emplace_back();
		players_.back().push_back(msg["player"]);
	}
	attachClientIdsToMessage();
}

bool GameManager::useEmptyRoom(const json & msg)
{
	for (size_t i = 0; i < active_games_.size(); ++i) {
		if (active_games_[i]->isEmpty()) {
			active_games_[i]->runGame(msg);
			players_[i].push_back(msg["player"]);
			attachClientIdsToMessage();
			return true;
		}
	}
	return false;
}

 //</Class GameManager>


