#include "pch.h"
#include "solver_api.h"
#include "Solver.h"
#include "cfr.h"

Solver::StrategyProfile* clientStratOOP = new Solver::StrategyProfile();
Solver::StrategyProfile* clientStratIP = new Solver::StrategyProfile();
std::vector<bool> hasCallOOP;
std::vector<bool> hasCallIP;

typedef std::vector<std::string> ClientRange;

ClientRange clientRangeOOP;
ClientRange clientRangeIP;

Solver::Bet call = Solver::Bet(1.0, true);
Solver::Bet fold = Solver::Bet(true);
Solver::Bet check = Solver::Bet(false);


void SetMaxBetDepth(int maxDepth) {

	std::vector<Solver::Street> streetList;
	streetList.push_back(Solver::FLOP);
	streetList.push_back(Solver::TURN);
	streetList.push_back(Solver::RIVER);

	std::vector<Solver::Bet> initialChecks;
	std::vector<Solver::Bet> initialFolds;
	initialChecks.push_back(check);
	initialFolds.push_back(fold);
	hasCallOOP = std::vector<bool>(maxDepth, false);
	hasCallIP = std::vector<bool>(maxDepth, false);
	for (Solver::Street s : streetList) {
		//Initialize checks.
		clientStratOOP->AddStrategy(s, initialChecks);
		clientStratIP->AddStrategy(s, initialChecks);
		//Initialize calls and folds.
		for (int d = 0; d < maxDepth; d++) {
			clientStratOOP->AddStrategy(s, initialFolds);
			
			clientStratIP->AddStrategy(s, initialFolds);
		}
	}
}

void AddBetToStrategy(Player player, Street street, int betDepth, std::string bet) {
	
	if (player == OOP) {
		if (!hasCallIP.at(betDepth)) {
			clientStratIP->AddToStrategy((Solver::Street) street, betDepth + 1, call);
			hasCallIP.at(betDepth) = true;
		}
		Solver::Bet newBet(bet);
		clientStratOOP->AddToStrategy((Solver::Street) street, betDepth, newBet);
	}
	else {
		if (!hasCallOOP.at(betDepth)) {
			clientStratOOP->AddToStrategy((Solver::Street) street, betDepth + 1, call);
			hasCallOOP.at(betDepth) = true;
		}
		Solver::Bet newBet(bet);
		clientStratIP->AddToStrategy((Solver::Street) street, betDepth, newBet);
	}
}

void AddHandToRange(Player player, std::string hand) {

	if (player == OOP) {
		clientRangeOOP.push_back(hand);
	}
	else {
		clientRangeIP.push_back(hand);
	}

}

using A = Solver::Bet;
using P = Solver::PokerPlayerNode;
using C = Solver::ChanceNode;

Solver* pSolver;
CfrTree<A, P, C, Solver>* pCfrSolver;

void InitSolver(std::string flop, float startPot, float effStack) {
	pSolver = new Solver(clientStratOOP, clientStratIP, clientRangeOOP, clientRangeIP,
						 flop, startPot, effStack);
	pCfrSolver = new CfrTree<A, P, C, Solver>(
			pSolver, Solver::ChanceNode()
		);
	pCfrSolver->ConstructTree();
}

void RunBaseCfr(int numIterations) {
	pCfrSolver->CFR(numIterations);
}

void RunBaseCfrWithAccuracy(float accuracy) {
	pCfrSolver->CFR_ToAccuracy(accuracy);
}

void RunChanceSamplingCfr(int numIterations) {
	pCfrSolver->MCCFR(numIterations);
}

void RunMCCFRAccuracy(float accuracy) {
	pCfrSolver->MCCFR_ToAccuracy(accuracy);
}

long TreeSize() {
	return pCfrSolver->TreeSize();
}

void PrintSolve() {
	pCfrSolver->PrintTree();
}




