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

PlayerDeck::PlayerDeck() {}

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
	Card card = vec[0];
	if (vec.size() == 2 && isHigher(vec[0], vec[1], superior)) {
		card = vec[1];
	}
	std::vector<int> correct_cards;
	for (size_t i = 0; i < deck_.size(); ++i) {
		if (isHigher(card, deck_[i], superior)) {
			for (size_t j = i; j < deck_.size(); ++j) {
				if (isHigher(card, deck_[j], superior)) {
					correct_cards.push_back(j);
				}
			}
			return correct_cards;
		}
	}
	for (size_t i = 0; i < deck_.size(); ++i) {
		if (!deck_[i].getIsUsed() && (deck_[i].getSuit() == card.getSuit() || card.getSuit() == superior)) {
			correct_cards.push_back(i);
		}
	}
	if (correct_cards.empty()) {
		for (size_t i = 0; i < deck_.size(); ++i) {
			if (!deck_[i].getIsUsed()) {
				correct_cards.push_back(i);
			}
		}
	}
	return correct_cards;
}

std::size_t PlayerDeck::getMaxValue() const
{
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

bool Score::setClaim(int claim)
{
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

int Player::getPlayerId() const
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

std::vector<Card> Deck::showBonusCards() const
{
	std::vector<Card> temp;
	std::vector<Card>::const_iterator it = deck_it_;
	while (it != deck_.end()) {
		temp.push_back(*it);
	}
	return temp;
}

void Deck::addBonusCards(Player & player)
{
	while (deck_it_ != deck_.end()) {
		player.getPlayerDeck().addCard(*deck_it_++);
	}
	deck_it_ = deck_.begin();
}

void Deck::shuffle()
{
	std::mt19937 engine(rd_());
	std::shuffle(deck_.begin(), deck_.end(), engine);
}

// </Class Deck>


// <Class PlayersCollection>

PlayersCollection::PlayersCollection()
	: players_it_(players_.begin())
	, highest_claimer_(players_.begin())
	, compulsory_claimer_(players_.begin())
{}

PlayersCollection::PlayersCollection(const PlayersCollection & other)
	: players_(other.players_)
	, highest_claimer_(players_.end())
	, players_it_(players_.end())
	, compulsory_claimer_(players_.end())
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
		highest_claimer_ = players_.begin();
		players_it_ = players_.begin();
		compulsory_claimer_ = players_.begin();
		// DO WYMIANY
		if (players_.size() == 1) {
			++players_it_;
			(*highest_claimer_).getScoreClass().setClaim(100);
		}
		// DO WYMIANY
		return true;
	}
	return false;
}


bool PlayersCollection::getNextPlayer()
{
	if (players_.size() != 0) {
		if (++players_it_ == players_.end()) {
			players_it_ = players_.begin();
		}
		if (players_it_ == highest_claimer_) {
			return false;
		}
		return true;
	}
	else throw std::logic_error("No players registered");
}

void PlayersCollection::getNextCompulsoryClaimer()
{
	if (players_.size() != 0) {
		if (++compulsory_claimer_ == players_.end()) {
			compulsory_claimer_ = players_.begin();
		}
	}
	else throw std::logic_error("No players registered");
}

players_it & PlayersCollection::getCurrentPlayer()
{
	if (players_.size() != 0) {
		return players_it_;
	}
	else throw std::logic_error("No players registered");
}

players_it & PlayersCollection::setCurrentPlayer(int player_id)
{
	players_it end_it = players_.end();
	for (players_it tmp = players_.begin(); tmp != end_it; ++tmp) {
		if ((*tmp).getPlayerId() == player_id) {
			players_it_ = tmp;
			return players_it_;
		}
	}
	throw std::out_of_range("No player with given id exists in current game");
}

players_it & PlayersCollection::getHighestClaimer()
{
	if (players_.size() != 0) {
		return highest_claimer_;
	}
	else throw std::logic_error("No players registered");
}

players_it & PlayersCollection::getCompulsoryClaimer()
{
	if (players_.size() != 0) {
		return compulsory_claimer_;
	}
	else throw std::logic_error("No players registered");
}

void PlayersCollection::setHighestClaimer(Player & highest_claimer)
{
	highest_claimer_ = std::find(players_.begin(), players_.end(), highest_claimer);
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

Player & PlayersCollection::getPlayer(int player_id)
{
	for (auto& i : players_) {
		if (i.getPlayerId() == player_id)
			return i;
	}
	throw std::out_of_range("No player found based on given player id");
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
	request_type request;
	json feedback;
	std::vector<int> players_ids;
	if (parse(msg["action"]) == DISCONNECT || parse(msg["action"]) == LEAVE) {
		for (players_it it = players_.getArray().begin(); it != players_.getArray().end(); ++it) {
			if ((*it).getPlayerId() == msg["player"]) {
				players_.getArray().erase(it);
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
							feedback["data"].push_back(i);
						}
					}
					request.push_back(feedback);
				}
				if (temp_stage == BIDDING) {
					stage_ = temp_stage;
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
				feedback["who"].push_back(i.getPlayerId());
			}
			request.push_back(feedback);
			if (temp_stage == ADDING) {
				break;
			}
			feedback.clear();
			players_.getNextPlayer();

			break;
		case CHAT :
			request.push_back(chatMessage(msg)); break;
			for (json i : request) {
				i.clear();
			}
			ret_val = true;
		default: break;
		}
		break;
	case BIDDING:
		switch (parse(msg["action"])) {
		case CHAT:
			request.push_back(chatMessage(msg));
			ret_val = true; break;
		default: break;
		}
		break;
	default: break;
	}
	 man_.pushMessage(request);
	 return ret_val;
}

int Room::parse(const std::string & str)
{
	json command = { { "add", ADD }, { "get", GET }, { "bid", BID },
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
	players_.getPlayer(player_id).setReady(isReady);
	if (players_.getArray().size() == MAX_PLAYERS) {
		for (auto i : players_.getArray()) {
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

stage Bidder::Bid(int player_id, int new_amount)
{
	if ((*players_.getCurrentPlayer()).getPlayerId() != player_id) {
		throw std::logic_error("Player trying to bid not at his turn");
	}
	if ((*players_.getHighestClaimer()).getScoreClass().getClaim() >= new_amount) {
		throw std::logic_error("Player bids less than current highest bid");
	}
	(*players_.getCurrentPlayer()).getScoreClass().setClaim(new_amount);
	players_.setHighestClaimer((*players_.getCurrentPlayer()));
	while ((*players_.getCurrentPlayer()).getScoreClass().getClaim() == -1) {
		players_.getNextPlayer();
	}
	if ((*players_.getCurrentPlayer()).getScoreClass().getClaim() ==
		(*players_.getHighestClaimer()).getScoreClass().getClaim()) {
		return DEALING;
	}
	return BIDDING;
}

void Bidder::giveAddCards()
{
	deck_.addBonusCards((*players_.getHighestClaimer()));
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
	if ( player_id == (*players_.getHighestClaimer()).getPlayerId() || (player_id == user_id_ &&
		counter == 1)) {
		throw std::logic_error("Giving card to the wrong player");
	}	
	players_.getPlayer(player_id).getPlayerDeck().addCard(
		(*players_.getHighestClaimer()).getPlayerDeck().playCard(card_number));
	user_id_ = players_.getPlayer(player_id).getPlayerId();
	if (++counter == TWO_CARDS) {
		return PLAYING;
	}
	return DEALING;
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
{}

Game::~Game()
{}

const Card & Game::playTurn(int player, std::size_t card)
{
	return players_.getPlayer(player).getPlayerDeck().playCard(card);
}

stage Game::manageTurn(int player, int card)
{
	if ((*players_.getCurrentPlayer()).getPlayerId() != player) {
		throw std::logic_error("Not player's turn to play a card");
	}
	vec_.emplace_back(std::make_pair(player, playTurn(player, card) ));
	players_.getNextPlayer();
	if (vec_.size() == MAX_PLAYERS) {
		players_.setCurrentPlayer(compareCardsAndPassToWinner());
		vec_.clear();
		if (++turn_counter_ == MAX_TURNS) {
			reset(); 
			return SUMMING_UP;
		}
	}
	return PLAYING;
}

int Game::setSuperiorSuit()
{
	int score = 0;
	for (auto& i : vec_) {
		if (i.first == current_starting_player_) {
			if (i.second.getFigure() == KING || i.second.getFigure() == QUEEN) {
				if (players_.getPlayer(current_starting_player_).getPlayerDeck().doesHavePair(
					i.second.getSuit())) {
					super_suit_ = i.second.getSuit();
					switch (super_suit_) {
					case SPADES: score = SPADES; break;
					case CLUBS: score = CLUBS; break;
					case DIAMONDS: score = DIAMONDS; break;
					case HEARTS: score = HEARTS; break;
					default: break;
					}
				}
			}
		}
	}
	return score;
}

int Game::compareCardsAndPassToWinner()
{
	figures superior_suit_figure = NOT_A_FIGURE;
	figures ordinary_figure = NOT_A_FIGURE;
	suits current_suit = NONE;
	int winning_player = -1;
	int score = setSuperiorSuit();

	if (super_suit_ != NONE) {
		for (auto& i : vec_) {
			if (i.second.getSuit() == super_suit_) {
				if (superior_suit_figure < i.second.getFigure()) {
					superior_suit_figure = i.second.getFigure();
					winning_player = i.first;
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
				i.second.getFigure() == superior_suit_figure) {
				superior_suit_figure = i.second.getFigure();
				winning_player = i.first;
			}
		}
	}
	for (auto& i : vec_) {
		score += i.second.getFigure();
	}
	players_.getPlayer(winning_player).getScoreClass().addToTurnScore(score);
	return winning_player;
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
		if (i.getPlayerId() == (*players_.getHighestClaimer()).getPlayerId()) {
			if (i.getScoreClass().getClaim() > i.getScoreClass().getTurnScore()) {
				i.getScoreClass().addScore(-1 * i.getScoreClass().getClaim());
			}
			else {
				i.getScoreClass().addScore(i.getScoreClass().getClaim());
			}
		}
		else {
			i.getScoreClass().addScore(i.getScoreClass().getTurnScore());
		}
		if (i.getScoreClass().getScore() >= POINTS_WINNING_CAP) {
			return ENDING;
		}
	}
	resetPlayersAtributes();
	return BIDDING;
}

void SumScore::resetPlayersAtributes()
{
	for (auto& i : players_.getArray()) {
		i.getScoreClass().clearTurnScore();
		i.getScoreClass().resetClaim();
		i.getPlayerDeck().clearDeck();
	}
	players_.getNextCompulsoryClaimer();
	players_.setHighestClaimer(*players_.getCompulsoryClaimer());
	players_.setCurrentPlayer((*players_.getCompulsoryClaimer()).getPlayerId());
	players_.getNextPlayer();
	for (auto& i : players_.getArray()) {
		i.getPlayerDeck().clearDeck();
	}
	deck_.shuffle();
	deck_.dealCards(players_.getArray());
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
	json msg = json::parse(message);
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
	 //Tutaj mo¿e byæ ró¿nie w zale¿noœci od tego
	 //jak bêdzie na poziomie Roomu obs³ugiwany brak dodania
}

// </Class GameManager>

