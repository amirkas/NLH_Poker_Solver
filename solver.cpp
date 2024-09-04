#include "pch.h"
#include "Solver.h"
#include "PokerTables.h"
#include "EvaluatorLibrary.h"

Solver::Solver (
	StrategyProfile* stratOOP, StrategyProfile* stratIP,
	std::vector<HoleCards>& rangeOOP, std::vector<HoleCards>& rangeIP,
	Board board, float startingPot, float effectiveStack
)
{
	mStrategyOOP = stratOOP;
	mStrategyIP = stratIP;
	mRangeOOP = new Range(rangeOOP);
	mRangeIP = new Range(rangeIP);
	mBoard = board;
	mStartingPot = startingPot;
	mEffectiveStack = effectiveStack;

	InitEvaluator();

	mDrawCardsTable = new DrawCardsTable(mRangeOOP, mRangeIP, mBoard);
	mWinnerTable = new WinnerTable(mRangeOOP, mRangeIP, mBoard, mDrawCardsTable);
}

float Solver::UtilityFunc(std::vector<HistoryNode> history) {

	auto reverseStart = history.rbegin();
	auto reverseEnd = history.rend();
	int depthFromEnd = 0;
	for (reverseStart; reverseStart != reverseEnd; reverseStart++) {
		
		HistoryNode node = *reverseStart;
		if (node.IsChanceNode()) {
			ChanceNode chanceNode = node.GetChanceNode();
			//Players were all in before river. Find board -> winner.

			int winner = this->mWinnerTable->GetShowdownWinner(chanceNode.mHoleCardsOOP,
															   chanceNode.mHoleCardsIP,
															   chanceNode.mPrevTurnCard,
															   chanceNode.mPrevRiverCard);
			
			float potDiff = (chanceNode.mPot - this->mStartingPot);
			/*std::cout << "all in " << winner * potDiff << "\n";*/
			return static_cast<float>(winner) * potDiff;
		}
		if (node.IsPlayerNode()) {
			PokerPlayerNode playerNode = node.GetPlayerNode();
			Bet lastAction = node.GetAction();
			if (lastAction.mIsFold) {
				int mul = playerNode.IsPlayerOne() ? -1 : 1;
				float potDiff = (playerNode.mPot - playerNode.mAmountToCall - this->mStartingPot);
				/*std::cout << "fold " << mul * potDiff << "\n";*/
				return mul * potDiff;
			}
			else {
				std::string turnCard = playerNode.mBoard.substr(0, 2);
				std::string rivercard = playerNode.mBoard.substr(2, 2);
				std::string oopCard = playerNode.mHoleCardsOOP;
				std::string ipCard = playerNode.mHoleCardsIP;
				

				int winner = this->mWinnerTable->GetShowdownWinner(oopCard, ipCard,
																   turnCard, rivercard);
				
				float potDiff = (playerNode.mPot + playerNode.mAmountToCall - this->mStartingPot);
				/*std::cout << "river call " << winner * potDiff << "\n";*/
				return winner * potDiff;
			}
		
		}
	}
}

Solver::Strategy Solver::GetPlayerStrategy(
	Player player, Street street, int betDepth
) {
	if (player == OOP) {
		return this->mStrategyOOP->GetStrategy(street, betDepth);
	}
	else if (player == IP) {
		return this->mStrategyIP->GetStrategy(street, betDepth);
	}
	else {
		return Strategy();
	}
}

Solver::Range* Solver::GetPlayerRange(Player player) {
	if (player == OOP) {
		return this->mRangeOOP;
	}
	else if (player == IP) {
		return this->mRangeIP;
	}
	else {
		return nullptr;
	}
}

Solver::Board* Solver::GetDrawCards(
	HoleCards& CardsIP, HoleCards& CardsOOP, Card& turnCard
) {
	return mDrawCardsTable->GetCardsToDraw(CardsIP, CardsOOP, turnCard);
}






