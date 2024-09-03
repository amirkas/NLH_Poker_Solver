#include "pch.h"
#include "Solver.h"



void Solver::StrategyProfile::AddFlopStrategy(Strategy& strat) {
	this->mFlopStrat.push_back(strat);
}

void Solver::StrategyProfile::AddTurnStrategy(Strategy& strat) {
	this->mTurnStrat.push_back(strat);
}

void Solver::StrategyProfile::AddRiverStrategy(Strategy& strat) {
	this->mRiverStrat.push_back(strat);
}

void Solver::StrategyProfile::AddStrategy(Street street, Strategy& strat) {
	switch (street) {
		case FLOP:
			this->mFlopStrat.push_back(strat);
			return;
		case TURN:
			this->mTurnStrat.push_back(strat);
			return;
		case RIVER:
			this->mRiverStrat.push_back(strat);
			return;
		default:
			return;
	}
}

Solver::Strategy Solver::StrategyProfile::GetFlopStrategy(int betDepth) {
	if (betDepth > mFlopStrat.size()) {
		return Strategy();
	}
	return mFlopStrat.at(betDepth);
}

Solver::Strategy Solver::StrategyProfile::GetTurnStrategy(int betDepth) {
	if (betDepth > mTurnStrat.size()) {
		return Strategy();
	}
	return mTurnStrat.at(betDepth);
}

Solver::Strategy Solver::StrategyProfile::GetRiverStrategy(int betDepth) {
	if (betDepth > mTurnStrat.size()) {
		return Strategy();
	}
	return mTurnStrat.at(betDepth);
}

Solver::Strategy Solver::StrategyProfile::GetStrategy(Street street, int betDepth) {
	switch (street) {
		case FLOP:
			return this->GetFlopStrategy(betDepth);
		case TURN:
			return this->GetTurnStrategy(betDepth);
		case RIVER:
			return this->GetRiverStrategy(betDepth);
		default:
			return Strategy();
	}
}

int Solver::StrategyProfile::GetStrategySize(Street street, int betDepth) {
	return GetStrategy(street, betDepth).size();
}

void Solver::StrategyProfile::AddToStrategy(Street street, int depth, Bet& bet) {
	switch (street) {
		case FLOP:
			this->mFlopStrat.at(depth).push_back(bet);
			return;
		case TURN:
			this->mTurnStrat.at(depth).push_back(bet);
			return;
		case RIVER:
			this->mRiverStrat.at(depth).push_back(bet);
			return;
		default:
			return;
	}
}