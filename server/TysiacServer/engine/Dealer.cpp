#include "Dealer.hpp"

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
	if (player_id == (players_.getPlayer(HIGHEST)).getPlayerId() || (player_id == user_id_ &&
		counter == 1)) {
		throw std::logic_error("Giving card to the wrong player");
	}
	Card tmp = players_.getPlayer(HIGHEST).getPlayersDeck().playCard(card_number);
	tmp.setIsUsed(false);
	players_.getPlayer(X, player_id).getPlayersDeck().addCard(tmp);
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
		{ "max", players_.getPlayer(HIGHEST).getPlayersDeck().getMaxValue(true) }
	};
	feedback["who"] = feedback["player"];
	return feedback;
}
