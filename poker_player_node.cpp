#include "pch.h"
#include "Solver.h"
#include "nodes.h"

Solver::Street Solver::PokerPlayerNode::CurrentStreet() const {
	if (this->mBoard.size() == 0) {
		return FLOP;
	}
	else if (this->mBoard.size() == 2) {
		return TURN;
	}
	else {
		return RIVER;
	}
}

std::vector<Solver::Bet> Solver::PokerPlayerNode::ActionList(Solver* gameInfo) const {

	int depth = this->mBetDepth;
	Street currStreet = this->CurrentStreet();
	Player currPlayer = this->mPlayer;
	return gameInfo->GetPlayerStrategy(currPlayer, currStreet, depth);
}

void Solver::PokerPlayerNode::SwitchPlayer() {
	if (this->IsPlayerOne()) {
		this->mPlayer = Solver::IP;
	}
	else {
		this->mPlayer = Solver::OOP;
	}
}


typedef ClientNode<Solver::Bet, Solver::PokerPlayerNode, Solver::ChanceNode> PlayerNodeChild;
typedef Solver::PokerPlayerNode PlayerNode;
typedef Solver::Bet Bet;


/**
 * @brief Gets child for a fold, which is a terminal node. 
 * @return ClientNode<Bet, PokerPlayerNode, ChanceNode>(Terminal Node)
		   used by the generic cfr lib.
 */
ClientNode<Solver::Bet, Solver::PokerPlayerNode, Solver::ChanceNode>
Solver::PokerPlayerNode::HandleFold(const PlayerNode& p, const Bet& b, Solver* g) {
	return PlayerNodeChild(b);
}

/**
 * @brief Gets child for a check by the OOP player -> IP will have action.
 * @return ClientNode<Bet, PokerPlayerNode, ChanceNode>(Terminal Node)
		   used by the generic cfr lib.
 */
ClientNode<Solver::Bet, Solver::PokerPlayerNode, Solver::ChanceNode>
Solver::PokerPlayerNode::HandleOopCheck(const PlayerNode& p,const Bet& b, Solver* g) {
	PlayerNode newPlayerNode(p);
	newPlayerNode.SwitchPlayer();
	return PlayerNodeChild(newPlayerNode, b);
}

/**
 * @brief Gets child for a check by the IP player -> Turn / River dealt or Showdown.
 * @return ClientNode<Bet, PokerPlayerNode, ChanceNode>(Terminal Node)
		   used by the generic cfr lib.
 */
ClientNode<Solver::Bet, Solver::PokerPlayerNode, Solver::ChanceNode>
Solver::PokerPlayerNode::HandleIpCheck(const PlayerNode& p, const Bet& b, Solver* g) {
	Solver::Street currStreet = p.CurrentStreet();
	std::string board = p.mBoard;
	std::string hole_cards_oop = p.mHoleCardsOOP;
	std::string hole_cards_ip = p.mHoleCardsIP;
	if (currStreet == Solver::FLOP || currStreet == Solver::TURN ) {
		Solver::ChanceNode nextStreetNode(board, Utils::NO_CARD,
										  hole_cards_oop, hole_cards_ip, p.mPot);
		return PlayerNodeChild(nextStreetNode, b);
	}
	else {
		//Terminal node for showdown at river.
		return PlayerNodeChild(b);
	}
}

/**
 * @brief Gets child for a non all in and all in calls before the river.
 *	      Which are chance nodees.
 * @return ClientNode<Bet, PokerPlayerNode, ChanceNode>(Chance Node)
		   used by the generic cfr lib.
 */
ClientNode<Solver::Bet, Solver::PokerPlayerNode, Solver::ChanceNode>
Solver::PokerPlayerNode::HandleFlopOrTurnCall(const PlayerNode& p, const Bet& b, Solver* g) {
	int newPot = p.mPot + p.mAmountToCall;
	Solver::Street currStreet = p.CurrentStreet();

	bool isAllIn = ( newPot >= g->mEffectiveStack );

	std::string board = p.mBoard;
	std::string hole_cards_oop = p.mHoleCardsOOP;
	std::string hole_cards_ip = p.mHoleCardsIP;

	Solver::ChanceNode nextStreetNode(board, Utils::NO_CARD, hole_cards_oop,
									  hole_cards_ip, p.mPot, isAllIn);
	return PlayerNodeChild(nextStreetNode, b);
}

/**
 * @brief Gets child for a call on the river, which is a terminal node for showdown.
 * @return ClientNode<Bet, PokerPlayerNode, ChanceNode>(Terminal Node)
		   used by the generic cfr lib.
 */
ClientNode<Solver::Bet, Solver::PokerPlayerNode, Solver::ChanceNode>
Solver::PokerPlayerNode::HandleRiverCall(const PlayerNode& p, const Bet& b, Solver* g) {
	return PlayerNodeChild(b);
}

/**
 * @brief Gets child for aggressive action (a bet). Opponent has next action.
 * @return ClientNode<Bet, PokerPlayerNode, ChanceNode>(Poker Player node)
		   used by the generic cfr lib.
 */
ClientNode<Solver::Bet, Solver::PokerPlayerNode, Solver::ChanceNode>
Solver::PokerPlayerNode::HandleBet(const PlayerNode& p, const Bet& b, Solver* g) {
	float newPot = b.NextPot(p.mPot, g->mEffectiveStack);
	PlayerNode opponentNode(p);
	opponentNode.SwitchPlayer();
	opponentNode.AddToPot(newPot - p.mPot);
	opponentNode.SetCallAmount(newPot - p.mPot);
	opponentNode.ElevateDepth();
	return PlayerNodeChild(opponentNode);
}


/**
 * @brief Function used by Generic CFR lib to find child for an action.
 * @param b Bet given by the Generic CFR lib. 
 * @param gameInfo Pointer to static game information.
 * @return ClientNode<Bet, PokerPlayerNode, ChanceNode> used by the generic cfr lib.
 */
PlayerNodeChild Solver::PokerPlayerNode::Child(Bet b, Solver* gameInfo) const {

	Street currentStreet = this->CurrentStreet();
	if (b.IsFold()) {
		return PokerPlayerNode::HandleFold(*this, b, gameInfo);
	}
	else if (b.IsCheck() && this->IsPlayerOne()) {
		return PokerPlayerNode::HandleOopCheck(*this, b, gameInfo);
	}
	else if (b.IsCheck() && !this->IsPlayerOne()) {
		return PokerPlayerNode::HandleIpCheck(*this, b, gameInfo);
	}
	else if (b.IsCall() && ( currentStreet == FLOP || currentStreet == TURN )) {
		return PokerPlayerNode::HandleFlopOrTurnCall(*this, b, gameInfo);
	}
	else if (b.IsCall() && ( currentStreet == RIVER )) {
		return PokerPlayerNode::HandleRiverCall(*this, b, gameInfo);
	}
	else if (b.IsBet()) {
		return PokerPlayerNode::HandleBet(*this, b, gameInfo);
	}
	return Bet();
}
