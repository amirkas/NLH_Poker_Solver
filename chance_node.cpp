#include "pch.h"
#include "Solver.h"
#include "cfr_tree_nodes.h"

Solver::Board* Solver::ChanceNode::GetDrawCards(Solver* staticGameInfo) {
	return staticGameInfo->GetDrawCards(
		this->mHoleCardsOOP, this->mHoleCardsOOP, this->mPrevTurnCard
	);
}

float Solver::ChanceNode::DrawUniformProb(Board* drawCards) {
	return 2.0f / static_cast<float>(drawCards->size());
}

typedef ClientNode<Solver::Bet, Solver::PokerPlayerNode, Solver::ChanceNode> ChanceNodeChild; 
typedef std::vector<ChanceNodeChild> ChanceNodeChildList;
typedef Solver::ChanceNode ChanceNode;
typedef Solver::PokerPlayerNode PlayerNode;


void Solver::ChanceNode::HandleDealingHandOOP(ChanceNodeChildList& list, const ChanceNode& p, Solver* g)  {
	
	Solver::Range* oopRange = g->GetPlayerRange(Solver::OOP);
	float uniformProb = 1.0f / static_cast<float>(oopRange->size());
	auto iRange = oopRange->begin();
	auto iEnd = oopRange->end();
	for (iRange; iRange < iEnd; iRange++) {
		Solver::HoleCards oopCards = *iRange;
		ChanceNode dealtOOP(oopCards, Utils::NO_CARD, g->mStartingPot);
		list.push_back(ChanceNodeChild(dealtOOP, uniformProb));
	}
}

void Solver::ChanceNode::HandleDealingHandIP(ChanceNodeChildList& list, const ChanceNode& p, Solver* g)  {
	Solver::Range* ipRange = g->GetPlayerRange(Solver::IP);
	float uniformProb = 1.0f / static_cast<float>(ipRange->size());
	auto iRange = ipRange->begin();
	auto iEnd = ipRange->end();
	for (iRange; iRange < iEnd; iRange++) {
		Solver::HoleCards ipCards = *iRange;
		std::string hole_cards_oop = p.mHoleCardsOOP;
		
		PlayerNode dealtIp(
			Solver::OOP, g->mStartingPot, 0.0, Utils::NO_CARD, 
			hole_cards_oop, ipCards, (uint8_t) 0
		);
		list.push_back(ChanceNodeChild(dealtIp, uniformProb));
	}
}


void Solver::ChanceNode::HandleFlopAllIn(ChanceNodeChildList& list, const ChanceNode& p, Solver* g)
{
	std::string hole_cards_oop = p.mHoleCardsOOP;
	std::string hole_cards_ip = p.mHoleCardsIP;
	Solver::Board* turnCards = g->GetDrawCards(hole_cards_oop, hole_cards_ip, Utils::NO_CARD);
	float uniformProb = ChanceNode::DrawUniformProb(turnCards);
	for (int iCard = 0; iCard < turnCards->size(); iCard += 2) {
		std::string turnCard = turnCards->substr(iCard, 2);
		ChanceNode turnChanceNode(p, turnCard);
		list.push_back(ChanceNodeChild(turnChanceNode, uniformProb));
	}
}

void Solver::ChanceNode::HandleTurnAllIn(ChanceNodeChildList& list, const ChanceNode& p, Solver* g)  {
	std::string hole_cards_oop = p.mHoleCardsOOP;
	std::string hole_cards_ip = p.mHoleCardsIP;
	std::string prev_turn_card = p.mPrevTurnCard;
	Solver::Board* riverCards = g->GetDrawCards(hole_cards_oop, hole_cards_ip, prev_turn_card);
	float uniformProb = ChanceNode::DrawUniformProb(riverCards);
	for (int iCard = 0; iCard < riverCards->size(); iCard += 2) {
		std::string riverCard = riverCards->substr(iCard, 2);
		ChanceNode riverChanceNode(p, riverCard);
		list.push_back(ChanceNodeChild(riverChanceNode, uniformProb));
	}
}

void Solver::ChanceNode::HandleRiverAllIn(ChanceNodeChildList& list, const ChanceNode& p, Solver* g)  {
	list.push_back(ChanceNodeChild(1.0));
}

void Solver::ChanceNode::HandleTurnDraw(ChanceNodeChildList& list, const ChanceNode& p, Solver* solver)
{
	std::string hole_cards_oop = p.mHoleCardsOOP;
	std::string hole_cards_ip = p.mHoleCardsIP;
	Solver::Board* turnCards = solver->GetDrawCards(hole_cards_oop, hole_cards_ip, Utils::NO_CARD);
	float uniformProb = ChanceNode::DrawUniformProb(turnCards);
	for (int iCard = 0; iCard < turnCards->size(); iCard += 2) {
		std::string turnCard = turnCards->substr(iCard, 2);
		PlayerNode dealtTurnCard(
			Solver::OOP, p.mPot, 0.0, turnCard,
			hole_cards_oop, hole_cards_ip, static_cast<uint8_t>(0)
		);
		list.push_back(ChanceNodeChild(dealtTurnCard, uniformProb));
	}
}

void Solver::ChanceNode::HandleRiverDraw(ChanceNodeChildList& list, const ChanceNode& p, Solver* g) {
	std::string hole_cards_oop = p.mHoleCardsOOP;
	std::string hole_cards_ip = p.mHoleCardsIP;
	std::string prev_turn_card = p.mPrevTurnCard;
	Solver::Board* riverCards = g->GetDrawCards(hole_cards_oop, hole_cards_ip, prev_turn_card);
	float uniformProb = ChanceNode::DrawUniformProb(riverCards);
	for (int iCard = 0; iCard < riverCards->size(); iCard += 2) {
		std::string riverCard = riverCards->substr(iCard, 2);
		Solver::Board newBoard = prev_turn_card + riverCard;

		PlayerNode dealtRiverCard(
			Solver::OOP, p.mPot, 0.0, newBoard,
			hole_cards_oop, hole_cards_ip, (uint8_t) 0
		);
		list.push_back(ChanceNodeChild(dealtRiverCard, uniformProb));
	}
}


ChanceNodeChildList Solver::ChanceNode::Children(Solver* staticInfo) const {

	
	ChanceNodeChildList allChildren;
	if (this->mHoleCardsOOP.empty()) {
		HandleDealingHandOOP(allChildren, *this, staticInfo);
	}
	else if (this->mHoleCardsIP.empty()) {
		HandleDealingHandIP(allChildren, *this, staticInfo);
	}
	else if (this->IsAllIn() && this->mPrevTurnCard.empty()) {
		HandleFlopAllIn(allChildren, *this, staticInfo);
	}
	else if (this->IsAllIn() && (this->mPrevTurnCard.size() == 2 && this->mPrevRiverCard.size() != 2)) {
		HandleTurnAllIn(allChildren, *this, staticInfo);
	}
	else if (this->IsAllIn() && this->mPrevTurnCard.size() == 2 && this->mPrevRiverCard.size() == 2) {
		HandleRiverAllIn(allChildren, *this, staticInfo);
	}
	else if (this->mPrevTurnCard.empty()) {
		HandleTurnDraw(allChildren, *this, staticInfo);
	}
	else if (this->mPrevTurnCard.size() == 2) {
		HandleRiverDraw(allChildren, *this, staticInfo);
	}
	return allChildren;
}
