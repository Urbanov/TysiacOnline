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
	deck_[deck_.size() - 1].setIsUsed(false);
}

const Card & PlayerDeck::playCard(unsigned card_number) const
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

bool PlayerDeck::findCard(figures figure, suits suit) const
{
	for (auto i : deck_) {
		if (i.getFigure() == figure && i.getSuit() == suit && i.getIsUsed() == false) {
			return true;
		}
	}
	return false;
}

void PlayerDeck::clearDeck()
{
	deck_.clear();
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
	: player_id_(player_id)
	, nick_(nick)
{}

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
// </class Player> 



// <Class Deck>

Deck::Deck(const std::vector<Card>& deck) : deck_(deck), deck_it_(deck_.begin()) {}

Deck::Deck(const Deck & other)
	: deck_(other.deck_)
	, deck_it_(deck_.begin())
{}

Deck::Deck()
{
	std::vector<figures> figure_ = { NINE, TEN, JACK, QUEEN, KING, ACE };
	std::vector<suits> suit_ = { DIAMONDS, CLUBS, HEARTS, SPADES };
	for (auto i : figure_) {
		for (auto j : suit_) {
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
	for (auto&& i : players) {
		i.getPlayerDeck().clearDeck();
		for (int j = 0; j < MAX_CARDS; ++j) {
			i.getPlayerDeck().addCard(*deck_it_++);
		}
	}
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
		if (players_.size() == 1) {
			++players_it_;
			(*highest_claimer_).getScoreClass().setClaim(100);
		}
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

Player & PlayersCollection::getPlayer(int player_id)
{
	for (auto& i : players_) {
		if (i.getPlayerId() == player_id)
			return i;
	}
	throw std::out_of_range("No player found based on given player id");
}

// </Class PlayerCollection>



// <Class Croupier>

Croupier::Croupier(int croupier_id, GameManager & man)
	: man_(man)
	, croupier_id_(croupier_id)
	, stage_(ADDING)
	, adder_(deck_, players_, *this)
	, bidder_(deck_, players_, *this)
	, dealer_(deck_, players_, *this)
	, game_(deck_, players_, *this)
	, score_(deck_, players_, *this)
{}

Croupier::Croupier(const Croupier & other)
	: man_(other.man_)
	, croupier_id_(other.croupier_id_)
	, deck_(other.deck_)
	, players_(other.players_)
	, stage_(other.stage_)
	, adder_(other.adder_)
	, bidder_(other.bidder_)
	, dealer_(other.dealer_)
	, game_(other.game_)
	, score_(other.score_)
{}

Croupier::~Croupier()
{}

void Croupier::changeStage(stage new_stage)
{
	stage_ = new_stage;
}

bool Croupier::runGame(const json & msg)
{
	bool ret_val = false;
	request_type request;
	json feedback;
	std::vector<int> players_ids;
	switch (stage_) {
	case ADDING :
		switch (parse(msg["action"])) {
		case ADD:
			if (adder_.addPlayer(msg["player"], msg["values"])) {
				ret_val = true;
				//for (auto i : players_.getArray()) {
				//	players_ids.push_back(i.getPlayerId());
				//}
				feedback["who"] = msg["player"];//players_ids;
				feedback["type"] = "update";
				feedback["action"] = "add";
				feedback["id"] = croupier_id_;
				feedback["player"] = msg["player"];
				feedback["values"] = msg["values"];
				request.push_back(feedback);
				feedback.clear();
				//if (adder_.isFull()) {
				//	feedback["type"] = "update";
				//	feedback["action"] = "get";
				//	for (auto i : players_.getArray()) {
				//		feedback["who"] = i.getPlayerId();
				//		for (auto j : i.getPlayerDeck().getDeck()) {
				//			feedback["values"].push_back({ j.getSuit(), j.getFigure() });
				//		}
				//		request.push_back(feedback);
				//		feedback.erase("who");
				//		feedback.erase("values");
				//	}
				//}
			}
			break;
		case CHAT :
			request.push_back(chatMessage(msg)); break;
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

int Croupier::parse(const std::string & str)
{
	json command = { { "add", ADD },{ "get", GET },{ "bid", BID },
					 { "play", PLAY },{ "chat", CHAT } 
	};
	return command[str];
}

json Croupier::chatMessage(const json & msg)
{
	json response = {
		{"action", "chat"},
		{"player", msg["player"]},
		{"values", msg["values"]},
	};
	for (auto i : players_.getArray()) {
		//std::cout << i.getPlayerId() << std::endl;
		if (i.getPlayerId() != msg["player"]) {
			response["who"].push_back(i.getPlayerId());
		}
	}
	return response;
}


// </Class Croupier>


// <Class Controller>
Controller::Controller(Deck & deck, PlayersCollection & players, Croupier & croup)
	: deck_(deck)
	, players_(players)
	, croupier_(croup)
{}

Controller::~Controller()
{}

// </Class Controller>



// <Class Adder>

Adder::Adder(Deck & deck, PlayersCollection & players, Croupier & croup) :
	Controller(deck, players, croup)
{}

Adder::~Adder()
{}

bool Adder::addPlayer(int player_id, std::string nick)
{
	bool ret_value = players_.addPlayer(player_id, nick);
	if (isFull()) {
		players_.getNextPlayer();
		for (int i = 0; i < 10; ++i) {
			players_.getNextPlayer();
		}
		croupier_.changeStage(BIDDING);
		deck_.dealCards(players_.getArray());
	}
	return ret_value;
}

bool Adder::isFull() const
{
	return (players_.getArray().size() == MAX_PLAYERS);
}

// </Class Adder>



// <Class Bidder>

Bidder::Bidder(Deck & deck, PlayersCollection & players, Croupier & croup) :  
	Controller(deck, players, croup)
{}

Bidder::~Bidder()
{}

bool Bidder::Bid(int player_id, int new_amount)
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
		croupier_.changeStage(DEALING);
		return false;
	}
	return true;
}

void Bidder::giveAddCards()
{
	deck_.addBonusCards((*players_.getHighestClaimer()));
}

// </Class Bidder>



// <Class Dealer>

Dealer::Dealer(Deck & deck, PlayersCollection & players, Croupier & croup) : 
	user_id_(-1), 
	counter(0), 
	Controller(deck, players, croup)
{}

Dealer::~Dealer()
{}

void Dealer::giveCardToPeer(int player_id, unsigned card_number)
{
	if ( player_id == (*players_.getHighestClaimer()).getPlayerId() || (player_id == user_id_ &&
		counter == 1)) {
		throw std::logic_error("Giving card to the wrong player");
	}	
	players_.getPlayer(player_id).getPlayerDeck().addCard(
		(*players_.getHighestClaimer()).getPlayerDeck().playCard(card_number));
	user_id_ = players_.getPlayer(player_id).getPlayerId();
	if (++counter == TWO_CARDS) {
		croupier_.changeStage(PLAYING);
	}
}

void Dealer::reset()
{
	user_id_ = -1;
	counter = 0;
}

// </Class Dealer>



// <Class Game>

Game::Game(Deck & deck, PlayersCollection & players, Croupier & croup)
	: turn_counter_(0)
	, super_suit_(NONE)
	, Controller(deck, players, croup)
{}

Game::~Game()
{}

auto Game::playTurn(int player, unsigned card)
{
	return players_.getPlayer(player).getPlayerDeck().playCard(card);
}

void Game::manageTurn(int player, int card)
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
			croupier_.changeStage(SUMMING_UP);
			reset();
		}
	}
}

int Game::setSuperiorSuit()
{
	int score = 0;
	for (auto i : vec_) {
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
		for (auto i : vec_) {
			if (i.second.getSuit() == super_suit_) {
				if (superior_suit_figure < i.second.getFigure()) {
					superior_suit_figure = i.second.getFigure();
					winning_player = i.first;
				}
			}
		}
	}
	if (superior_suit_figure == NOT_A_FIGURE) {
		for (auto i : vec_) {
			if (i.first == current_starting_player_) {
				superior_suit_figure = i.second.getFigure();
				current_suit = i.second.getSuit();
			}
		}
		for (auto i : vec_) {
			if (i.second.getSuit() == current_suit && 
				i.second.getFigure() == superior_suit_figure) {
				superior_suit_figure = i.second.getFigure();
				winning_player = i.first;
			}
		}
	}
	for (auto i : vec_) {
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

SumScore::SumScore(Deck & deck, PlayersCollection & players, Croupier & croup) : 
	Controller(deck, players, croup)
{}

SumScore::~SumScore()
{}

void SumScore::sumUpScore()
{
	for (auto i : players_.getArray()) {
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
			croupier_.changeStage(ENDING);
		}
	}
	resetPlayersAtributes();
}

void SumScore::resetPlayersAtributes()
{
	for (auto i : players_.getArray()) {
		i.getScoreClass().clearTurnScore();
		i.getScoreClass().resetClaim();
		i.getPlayerDeck().clearDeck();
	}
	players_.getNextCompulsoryClaimer();
	players_.setHighestClaimer(*players_.getCompulsoryClaimer());
	players_.setCurrentPlayer((*players_.getCompulsoryClaimer()).getPlayerId());
	players_.getNextPlayer();
	for (auto i : players_.getArray()) {
		i.getPlayerDeck().clearDeck();
	}
	deck_.shuffle();
	deck_.dealCards(players_.getArray());
	croupier_.changeStage(BIDDING);
}

// </Class SumScore>


GameManager::GameManager() : croupier_counter_(0)
{}

GameManager::~GameManager()
{}

req GameManager::doWork(const std::string & message)
{
	req vec;
	json msg = json::parse(message);
	if (msg["action"] == "add") {
		for (auto&& i : active_games_) {
			if (i->runGame(msg)) {
				for (auto j : feedback_) {
					std::vector<int> v;
					for (auto a : j["who"]) {
						v.push_back(static_cast<int>(a));
					}
					vec.emplace_back(std::make_pair(j.dump(), v));
				}
				return vec;
			}
		}
		//active_games_.emplace_back(croupier_counter_++, *this);
		active_games_.push_back(new Croupier(croupier_counter_++, *this));
		active_games_.back()->runGame(msg);
		for (auto&& i : feedback_) {
			std::vector<int> v;
			for (auto a : i["who"]) {
				v.push_back(static_cast<int>(a));
			}
			vec.emplace_back(std::make_pair(i.dump(), v));
		}
	}
	else {
		active_games_[msg["id"]]->runGame(msg);
		for (auto&& i : feedback_) {
			std::vector<int> v;
			for (auto a : i["who"]) {
				//std::cout << a << std::endl;
				v.push_back(static_cast<int>(a));
			}
			vec.emplace_back(std::make_pair(i.dump(), v));
		}
	}
	return vec;
}

void GameManager::pushMessage(const request_type & msg)
{
	feedback_ = msg;
}