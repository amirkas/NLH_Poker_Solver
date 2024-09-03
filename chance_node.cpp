#include "pch.h"
#include "Solver.h"
#include "cfr_tree_nodes.h"

Solver::Board* Solver::ChanceNode::GetDrawCards(Solver* staticGameInfo) {
	return staticGameInfo->GetDrawCards(
		this->mHoleCardsOOP, this->mHoleCardsOOP, this->mPrevTurnCard
	);
}

float Solver::ChanceNode::DrawUniformProb(Board* drawCards) {
	return 1.0 / (float) drawCards->size();
}

typedef ClientNode<Solver::Bet, Solver::PokerPlayerNode, Solver::ChanceNode> ChanceNodeChild; 
typedef std::vector<ChanceNodeChild> ChanceNodeChildList;
typedef Solver::ChanceNode ChanceNode;
typedef Solver::PokerPlayerNode PlayerNode;

static void HandleGameStart(ChanceNodeChildList& list, ChanceNode& p, Solver* g) {

}

static void HandleDealingHandOOP(ChanceNodeChildList& list, ChanceNode& p, Solver* g) {
	
	Solver::Range* oopRange = g->GetPlayerRange(Solver::OOP);
	float uniformProb = 1.0 / (float) oopRange->size();
	auto iRange = oopRange->begin();
	auto iEnd = oopRange->end();
	for (iRange; iRange < iEnd; iRange++) {
		Solver::HoleCards oopCards = *iRange;
		ChanceNode dealtOOP(oopCards, Utils::NO_CARD, g->mStartingPot);
		list.push_back(ChanceNodeChild(dealtOOP, uniformProb));
	}
}

static void HandleDealingHandIP(ChanceNodeChildList& list, ChanceNode& p, Solver* g) {
	Solver::Range* ipRange = g->GetPlayerRange(Solver::IP);
	float uniformProb = 1.0 / (float) ipRange->size();
	auto iRange = ipRange->begin();
	auto iEnd = ipRange->end();
	for (iRange; iRange < iEnd; iRange++) {
		Solver::HoleCards ipCards = *iRange;
		PlayerNode dealtIp(
			Solver::OOP, g->mStartingPot, 0.0, Utils::NO_CARD, 
			p.mHoleCardsOOP, ipCards, (uint8_t) 0
		);
		list.push_back(ChanceNodeChild(dealtIp, uniformProb));
	}
}

static void HandleFlopAllIn(ChanceNodeChildList& list, ChanceNode& p, Solver* g) {

	Solver::Board* turnCards = g->GetDrawCards(p.mHoleCardsOOP, p.mHoleCardsIP, Utils::NO_CARD);
	float uniformProb = p.DrawUniformProb(turnCards);
	for (int iCard = 0; iCard < turnCards->size(); iCard += 2) {
		std::string turnCard = turnCards->substr(iCard, 2);
		ChanceNode turnChanceNode(p, turnCard);
		list.push_back(ChanceNodeChild(turnChanceNode, uniformProb));
	}
}

static void HandleTurnAllIn(ChanceNodeChildList& list, ChanceNode& p, Solver* g) {

	Solver::Board* riverCards = g->GetDrawCards(p.mHoleCardsOOP, p.mHoleCardsIP, p.mPrevTurnCard);
	float uniformProb = p.DrawUniformProb(riverCards);
	for (int iCard = 0; iCard < riverCards->size(); iCard += 2) {
		std::string riverCard = riverCards->substr(iCard, 2);
		ChanceNode riverChanceNode(p, riverCard);
		list.push_back(ChanceNodeChild(riverChanceNode, uniformProb));
	}
}

static void HandleRiverAllIn(ChanceNodeChildList& list, ChanceNode& p, Solver* g) {
	list.push_back(ChanceNodeChild(1.0));
}

static void HandleTurnDraw(ChanceNodeChildList& list, ChanceNode& p, Solver* g) {
	Solver::Board* turnCards = g->GetDrawCards(p.mHoleCardsOOP, p.mHoleCardsIP, Utils::NO_CARD);
	float uniformProb = p.DrawUniformProb(turnCards);
	for (int iCard = 0; iCard < turnCards->size(); iCard += 2) {
		std::string turnCard = turnCards->substr(iCard, 2);
		PlayerNode dealtTurnCard(
			Solver::OOP, p.mPot, 0.0, turnCard,
			p.mHoleCardsOOP, p.mHoleCardsIP, (uint8_t) 0
		);
		list.push_back(ChanceNodeChild(dealtTurnCard, uniformProb));
	}
}

static void HandleRiverDraw(ChanceNodeChildList& list, ChanceNode& p, Solver* g) {
	Solver::Board* riverCards = g->GetDrawCards(p.mHoleCardsOOP, p.mHoleCardsIP, p.mPrevTurnCard);
	float uniformProb = p.DrawUniformProb(riverCards);
	for (int iCard = 0; iCard < riverCards->size(); iCard += 2) {
		std::string riverCard = riverCards->substr(iCard, 2);
		Solver::Board newBoard = p.mPrevTurnCard + riverCard;
		PlayerNode dealtRiverCard(
			Solver::OOP, p.mPot, 0.0, newBoard,
			p.mHoleCardsOOP, p.mHoleCardsIP, (uint8_t) 0
		);
		list.push_back(ChanceNodeChild(dealtRiverCard, uniformProb));
	}
}

ChanceNodeChildList Solver::ChanceNode::Children(Solver* staticInfo) {

	ChanceNodeChildList allChildren;
	if (this->mHoleCardsOOP.size() == 0) {
		HandleDealingHandOOP(allChildren, *this, staticInfo);
	}
	else if (this->mHoleCardsIP.size() == 0) {
		HandleDealingHandIP(allChildren, *this, staticInfo);
	}
	else if (this->IsAllIn() && this->mPrevTurnCard.size() == 0) {
		HandleFlopAllIn(allChildren, *this, staticInfo);
	}
	else if (this->IsAllIn() && this->mPrevTurnCard.size() == 2) {
		HandleTurnAllIn(allChildren, *this, staticInfo);
	}
	else if (this->IsAllIn() && this->mPrevTurnCard.size() == 4) {
		HandleRiverAllIn(allChildren, *this, staticInfo);
	}
	else if (this->mPrevTurnCard.size() == 0) {
		HandleTurnDraw(allChildren, *this, staticInfo);
	}
	else {
		HandleRiverDraw(allChildren, *this, staticInfo);
	}
	return allChildren;
}
