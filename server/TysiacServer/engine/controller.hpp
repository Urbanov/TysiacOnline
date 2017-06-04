#pragma once
#include "deck.hpp"
#include "players_collection.hpp"

class Controller;

using PController = std::unique_ptr<Controller>;
using req = std::vector<std::pair<std::string, std::vector<int>>>;


/**
 * @brief commands used in messages sent to and received from players
 */
enum commands {
	LEAVE = 0,
	DISCONNECT = 15,
	READY = 12,
	ADD = 2,
	DEAL = 4,
	BID = 3,
	PLAY = 5,
	CHAT = 1
};

/**
 * @brief possible server's stages
 */
enum stage {
	FAIL,
	LEAVING,
	ADDING,
	STARTING,
	BIDDING,
	DEALING,
	PLAYING,
	SUMMING_UP,
	ENDING
};

/**
 * @brief abstract class, part of controller
 */
class Controller {
public:
	Controller(Deck& deck, PlayersCollection& players);
	virtual ~Controller();
	/**
	 * @brief method used to parse every type of message
	 *
	 * @param msg player's message
	 * @param stage_ current server's stage
	 */
	virtual stage changeModel(const json& msg, const stage stage_) = 0;
	/**
	* @brief method used to create reponse to every type of player's message
	*
	* @param msg player's message
	* @param stage_ current server's stage
	*/
	virtual request_type createMessages(const json& msg, const stage stage_) = 0;
	/**
	 * @brief resets all members in case of ending game
	 */
	virtual void reset();
protected:
	Deck& deck_;
	PlayersCollection& players_;
};