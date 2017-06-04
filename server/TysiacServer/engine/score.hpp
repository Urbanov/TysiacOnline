#pragma once
#include <stdexcept>

const int NO_TAX_CAP = 800;

/**
 * @brief class representing player's score
 */
class Score {
public:
	Score();
	~Score();
	int getScore() const;
	void addScore(int score);
	void addToTurnScore(int temp_score);
	int getTurnScore() const;
	bool setClaim(int claim, bool isFinal);
	int getClaim() const;
	void reset(bool isFinal);
	int round(int number) const;
	void roundScore();
private:

	int score_; /// player's overall score
	int temp_score_; /// player's turn score
	int claim_; /// player's claim's value
};