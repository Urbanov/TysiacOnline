#include "dealer.hpp"

Dealer::Dealer(Deck& deck, PlayersCollection& players)
	: user_id_(-1)
	, counter(0)
	, Controller(deck, players)
{}

Dealer::~Dealer()
{}

/**
 * @brief responsible for parsing message and passing card from one player to another
 *
 * @param msg message from player who gives away a card
 * @param stage_ current server's stage
 *
 * @return new server's stage
 */
stage Dealer::changeModel(const json& msg, const stage stage_)
{
	for (const auto& i : msg["data"]) {
		giveCardToPeer(i["player"], i["card"]);
	}
	return DEALING;
}

/**
 * @brief Create messages that will be sent to users
 *
 * @param msg message from player 
 */
request_type Dealer::createMessages(const json& msg, stage stage_)
{
	request_type request = createMessage(msg);
	request.push_back(createFinalBidMessage());
	return request;
}

/**
 * @brief removes card from owner's deck and adds it to another player
 *
  *@param player_id id of a new owner
 * @param card_number index of tramsfered card in owner's deck
 */
stage Dealer::giveCardToPeer(int player_id, std::size_t card_number)
{
	if (player_id == (players_.getPlayer(HIGHEST)).getPlayerId() || (player_id == user_id_ &&
		counter == 1)) {
		throw std::logic_error("Giving card to the wrong player");
	}
	Card tmp = players_.getPlayer(HIGHEST).getPlayersDeck().playCard(card_number);
	tmp.setIsUsed(false);
	players_.getPlayer(X, player_id).getPlayersDeck().addCard(tmp);
	user_id_ = static_cast<int>(players_.getPlayer(X, player_id).getPlayerId());
	if (++counter == TWO_CARDS) {
		return PLAYING;
	}
	return DEALING;
}

/**
 * @brief calling Deck::dealcards()
 */
void Dealer::dealCards()
{
	//deck_.shuffle();
	deck_.dealCards(players_.getArray());
}

/**
 * @brief creates messages containing information about the result of dealing cards for other players in a room
 *
 * @param msg message from player
 * @return vector of created messages
 */
request_type Dealer::createMessage(const json& msg) const
{
	request_type request;
	for (const auto& i : msg["data"]) {
		json feedback = {
			{ "action", "deal" },
			{ "who", i["player"] }
		};
		Card tmp = players_.getPlayer(X, i["player"]).getPlayersDeck().getDeck().back();
		json tmp1 = {
			{ "figure", tmp.getFigure() },
			{ "suit", tmp.getSuit() }
		};
		feedback["data"].push_back(tmp1);
		request.push_back(feedback);
	}
	return request;
}

/**
 *@ brief used after every round
 */
void Dealer::reset()
{
	user_id_ = -1;
	counter = 0;
}

/**
 * @brief create message for bidding winner, calling him to make final bid
 *
 * @return message containing information about his maximum and minimum possible bid
 */
json Dealer::createFinalBidMessage() const
{
	json feedback;
	feedback["action"] = "bid";
	feedback["player"] = players_.getPlayer(HIGHEST).getPlayerId();
	feedback["data"] = {
		{ "value", players_.getPlayer(HIGHEST).getScoreClass().getClaim() },
		{ "id", feedback["player"] },
		{ "min", players_.getPlayer(HIGHEST).getScoreClass().getClaim() },
		{ "max", players_.getPlayer(HIGHEST).getPlayersDeck().getMaxValue(true) }
	};
	feedback["who"] = feedback["player"];
	return feedback;
}
