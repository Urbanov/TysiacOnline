#pragma once
#include <stdexcept>

const int NO_TAX_CAP = 800;

class Score {
public:
	Score();
	~Score();
	int getScore() const;
	void addScore(int);
	void addToTurnScore(int);
	int getTurnScore() const;
	bool setClaim(int, bool);
	int getClaim() const;
	void reset(bool);
	int round(int) const;
	void roundScore();
private:

	int score_;
	int temp_score_;
	int claim_;
};