#include "pch.h"
#include "PokerUtils.h"
#include "Solver.h"

Solver::Range::Range(std::string& rangeStr) {

	if (rangeStr.size() % 4 == 0) {
		
		for (int iHoleCard = 0; iHoleCard < rangeStr.size(); iHoleCard += 4) {
			std::string holeCards = rangeStr.substr(iHoleCard, 4);
			mRange.push_back(holeCards);
		}
	}
	else {
		mRange = std::vector<HoleCards>();
	}
}

Solver::Range Solver::Range::GetPossibleRange(
	HoleCards& currPlayerHand, Range* opponentRange, Board& currBoard
) {
	Range possibleRange;
	std::string seenCards = currPlayerHand + currBoard;
	int initialRemainingSize = Utils::DECK_SIZE - seenCards.size();
	auto iHoleCardOpp = opponentRange->begin();
	auto iEndOpp = opponentRange->end();

	for (iHoleCardOpp; iHoleCardOpp < iEndOpp; iHoleCardOpp++) {
		std::string total = seenCards + *(iHoleCardOpp);
		std::string remaining = Utils::RemainingCards(total);
		if (remaining.size() == initialRemainingSize - 2) {
			possibleRange.push_back(*iHoleCardOpp);
		}
	}
	return possibleRange;
}

int Solver::Range::HandIndex(HoleCards hand) {
	Range::iterator elem = std::find(this->begin(), this->end(), hand);
	if (elem == this->end()) {
		return -1;
	}
	else {
		return (int) (elem - this->begin());
	}
}