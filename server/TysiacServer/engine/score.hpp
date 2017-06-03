#pragma once
#include <stdexcept>

const int NO_TAX_CAP = 800;

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

	int score_;
	int temp_score_;
	int claim_;
};