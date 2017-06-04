#include "score.hpp"

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

/**
 * @brief adds points to player's score
 *
 * @param score amount of points to add
 */
void Score::addScore(int score)
{
	score_ += score;
}


/**
* @brief adds points to player's temporary score
*
* @param temp_score amount of points to add
*/
void Score::addToTurnScore(int temp_score)
{
	temp_score_ += temp_score;
}

int Score::getTurnScore() const
{
	return temp_score_;
}

/**
 * @brief resets points player has achieved (either temporary or all)
 *
 * @param isFinal reset depends on the situation, if game is over isFinal is set to true and all score is cleared
 */
void Score::reset(bool isFinal)
{
	temp_score_ = 0;
	claim_ = 0;
	if (isFinal) {
		score_ = 0;
	}
}

/**
 * @brief set player's new claim, can be only higher than previous
 * 
 * @param claim new value to set
 * @param isFinal set to true if it's final bid after regular bidding
 */
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


/**
 * @brief calculates whether player achieved claimed bid value or not
 */
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

/**
 * @brief round turn score to value divisible by ten
 */
int Score::round(int number) const
{
	int temp = number % 10;
	if (temp != 0) {
		if (temp >= 5) {
			number += (10 - temp);
		}
		else {
			number -= temp;
		}
	}
	return number;
}

