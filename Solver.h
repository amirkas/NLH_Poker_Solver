#pragma once
#include <vector>
#include <string>
#include "PokerUtils.h"
#include "PokerTables.h"
#include "cfr.h"
#include "nodes.h"


class DrawCardsTable;
class WinnerTable;

class Solver {

public:

	using Board = std::string;
	using HoleCards = std::string;
	using Card = std::string;
	enum Player { OOP, IP };
	enum Street { FLOP, TURN, RIVER };

	class Bet;
	class PokerPlayerNode;
	class ChanceNode;
	class Range;
	class StrategyProfile;

	using Strategy = std::vector<Bet>;

	Range* mRangeOOP;
	Range* mRangeIP;
	StrategyProfile* mStrategyOOP;
	StrategyProfile* mStrategyIP;
	Board mBoard;
	float mStartingPot;
	float mEffectiveStack;
	DrawCardsTable* mDrawCardsTable;
	WinnerTable* mWinnerTable;

	

	Solver(
		StrategyProfile* stratOOP, StrategyProfile* stratIP, std::vector<HoleCards>& rangeOOP,
		std::vector<HoleCards>& rangeIP, Board board, float startingPot, float effectiveStack
	);
	

	Strategy GetPlayerStrategy(Player player, Street street, int betDepth);
	Range* GetPlayerRange(Player player);
	Board* GetDrawCards(HoleCards& CardsIP, HoleCards& CardsOOP, Card& turnCard);

	class Range {
	public:
		std::vector<HoleCards> mRange;
		using iterator = typename std::vector<HoleCards>::iterator;
		using reverse_iterator = typename std::vector<HoleCards>::reverse_iterator;

		Range() {}
		Range(std::vector<HoleCards>& range) : mRange{ range } {}
		Range(std::string& rangeStr);

		iterator begin() { return mRange.begin(); }
		iterator end() { return mRange.end(); }
		reverse_iterator rbegin() { return mRange.rbegin(); }
		reverse_iterator rend() { return mRange.rend(); }
		HoleCards at(int iHand) { return mRange.at(iHand); }
		size_t size() { return mRange.size(); }
		void push_back(HoleCards& holeCards) { mRange.push_back(holeCards); }

		Range GetPossibleRange (
			HoleCards& currPlayerHand, Range* opponentRange, Board& currBoard
		);

		int HandIndex(HoleCards hand);
	};

	using Strategy = std::vector<Bet>;

	class StrategyProfile {

		std::vector<Strategy> mFlopStrat;
		std::vector<Strategy> mTurnStrat;
		std::vector<Strategy> mRiverStrat;

	public:
		StrategyProfile() {}

		Strategy GetFlopStrategy(int betDepth);
		Strategy GetTurnStrategy(int betDepth);
		Strategy GetRiverStrategy(int betDepth);

		Strategy GetStrategy(Street street, int betDepth);
		int GetStrategySize(Street street, int betDepth);

		void AddFlopStrategy(Strategy& strat);
		void AddTurnStrategy(Strategy& strat);
		void AddRiverStrategy(Strategy& strat);

		void AddStrategy(Street, Strategy& strat);
		void AddToStrategy(Street s, int betDepth, Bet& b);
	};

	/**
		* @brief Classes used by generic cfr library for game tree construciton.
		*/


	class Bet {

	public:
		bool mIsBet;
		bool mIsCall;
		bool mIsFold;
		bool mIsCheck;

		float mBetAmount;

		Bet() : mIsBet(false), mIsCall(false), mIsFold(false),
			mIsCheck(true), mBetAmount(0.0) {}

		Bet(float betAmount, bool isCall) :
			mBetAmount(betAmount), mIsCall(isCall),
			mIsBet(!isCall), mIsCheck(false), mIsFold(false) {}

		Bet(bool isFold) :
			mIsBet(false), mIsCall(false), mIsFold(isFold),
			mIsCheck(!isFold), mBetAmount(0.0) {}

		Bet(std::string betStr) :
			Bet{} {
			if (betStr.size() > 1) {
				if (betStr[0] == 'b') {
					mIsBet = true;
					mIsCheck = false;
					mBetAmount = std::stof(betStr.substr(1));
				}
			}
		}

		bool IsFold() { return mIsFold; }
		bool IsCheck() { return mIsCheck; }
		bool IsCall() { return mIsCall; }
		bool IsBet() { return mIsBet; }

		float NextPot(float currPot, float potLimit) const {
			if (mIsCheck || mIsFold) {
				return currPot;
			}
			else if (mIsBet) {
				float ceilingDiff = potLimit - currPot;
				float betAmount = mBetAmount * currPot;
				return currPot + (std::min)(ceilingDiff / 2, betAmount);
			}
			else if (mIsCall) {
				return currPot + (mBetAmount * currPot);
			}
			else
			{
				return currPot;
			}
		}
		std::string ToHash() const { return Utils::NO_CARD; }
	};

	class PokerPlayerNode {
	public:
		Player mPlayer;
		float mPot;
		float mAmountToCall;
		Board mBoard;
		HoleCards mHoleCardsOOP;
		HoleCards mHoleCardsIP;
		uint8_t mBetDepth;

		PokerPlayerNode()
			: mPlayer{ OOP }, mPot{ 0.0 }, mAmountToCall{ 0.0 },
			mBoard{ Utils::NO_CARD }, mHoleCardsOOP{ Utils::NO_CARD },
			mHoleCardsIP{ Utils::NO_CARD }, mBetDepth{ 0 } {}

		PokerPlayerNode(
			Player player, float pot, float amountToCall, Board& board,
			HoleCards& holeCardsOOP, HoleCards& holeCardsIp, uint8_t betDepth
		)
			: mPlayer{ player }, mPot{ pot }, mAmountToCall { amountToCall },
			mBoard{ board }, mHoleCardsOOP{ holeCardsOOP },
			mHoleCardsIP{ holeCardsIp }, mBetDepth{ betDepth } {}

		PokerPlayerNode(const PokerPlayerNode& other) {
			mPlayer = other.mPlayer;
			mPot = other.mPot;
			mAmountToCall = other.mAmountToCall;
			mBoard = other.mBoard;
			mHoleCardsOOP = other.mHoleCardsOOP;
			mHoleCardsIP = other.mHoleCardsIP;
			mBetDepth = other.mBetDepth;
		}

		PokerPlayerNode(
			const PokerPlayerNode& other, Player player,
			float pot, float amountToCall, uint8_t betDepth
		)
			: PokerPlayerNode{ other } 
		{
			mPlayer = player;
			mPot = pot;
			mAmountToCall = amountToCall;
			mBetDepth = betDepth;
		}
		
		Street CurrentStreet() const;
		void SwitchPlayer();
		void SetCallAmount(float amountToCall) { this->mAmountToCall = amountToCall;}
		void AddToPot(float extraPotAmount) { this->mPot += extraPotAmount; }
		void ElevateDepth() { this->mBetDepth++; }

		std::string ToHash() const { return std::to_string(mPot) + mBoard + mHoleCardsIP + mHoleCardsOOP; }
		std::string ToInfoSetHash() const {
			std::string base = std::to_string(mPot) + mBoard;
			return mPlayer == OOP ? base + mHoleCardsOOP : base + mHoleCardsIP;
		}
		bool IsPlayerOne() const { return mPlayer == OOP; }
		std::vector<Bet> ActionList(Solver* staticInfo) const;
		static ClientNode<Bet, PokerPlayerNode, ChanceNode> HandleFold(const PokerPlayerNode& p, const Bet& bet, Solver* g);
		static ClientNode<Bet, PokerPlayerNode, ChanceNode> HandleOopCheck(const PokerPlayerNode& p, const Bet& bet, Solver* g);
		static ClientNode<Bet, PokerPlayerNode, ChanceNode> HandleIpCheck(const PokerPlayerNode& p, const Bet& bet, Solver* g);
		static ClientNode<Bet, PokerPlayerNode, ChanceNode> HandleFlopOrTurnCall(const PokerPlayerNode& p, const Bet& bet, Solver* g);
		static ClientNode<Bet, PokerPlayerNode, ChanceNode> HandleRiverCall(const PokerPlayerNode& p, const Bet& bet, Solver* g);
		static ClientNode<Bet, PokerPlayerNode, ChanceNode> HandleBet(const PokerPlayerNode& p, const Bet& bet, Solver* g);
		ClientNode<Bet, PokerPlayerNode, ChanceNode> Child(Bet b, Solver* staticInfo) const;

	};

	class ChanceNode {
	public:
		Card mPrevTurnCard;
		Card mPrevRiverCard;
		HoleCards mHoleCardsOOP;
		HoleCards mHoleCardsIP;
		float mPot;
		bool mIsAllIn;

		
		ChanceNode() :
			mPrevTurnCard{ Utils::NO_CARD }, mPrevRiverCard{ Utils::NO_CARD },
			mHoleCardsOOP{ Utils::NO_CARD }, mHoleCardsIP{ Utils::NO_CARD },
			mPot{ 0.0 }, mIsAllIn{ false } {}

		ChanceNode(const ChanceNode& other) {
			mPrevTurnCard = other.mPrevTurnCard;
			mPrevRiverCard = other.mPrevRiverCard;
			mHoleCardsOOP = other.mHoleCardsOOP;
			mHoleCardsIP = other.mHoleCardsIP;
			mPot = other.mPot;
			mIsAllIn = other.mIsAllIn;
		}

		ChanceNode(const ChanceNode& other, Card& turnCard) : ChanceNode{ other } {
			mPrevTurnCard = turnCard;
		}

		ChanceNode(
			Card& prevTurnCard, Card& prevRiverCard,
			HoleCards& holeCardsOOP, HoleCards& holeCardsIP, float prevPot
		) : mPrevTurnCard{ prevTurnCard }, mPrevRiverCard{ prevRiverCard },
			mHoleCardsOOP{ holeCardsOOP }, mHoleCardsIP{ holeCardsIP },
			mPot{ prevPot}, mIsAllIn { false } {}

		ChanceNode(
			HoleCards& holeCardsOOP, HoleCards& holeCardsIP, float prevPot
		) : mPrevTurnCard{ Utils::NO_CARD }, mPrevRiverCard{ Utils::NO_CARD },
			mHoleCardsOOP{ holeCardsOOP }, mHoleCardsIP{ holeCardsIP },
			mPot{ prevPot }, mIsAllIn{ false } {}


		ChanceNode(
			Card& prevTurnCard, Card& prevRiverCard,
			HoleCards& holeCardsOOP, HoleCards& holeCardsIP, float prevPot,
			bool isAllIn
		) : mPrevTurnCard{ prevTurnCard }, mPrevRiverCard{ prevRiverCard },
			mHoleCardsOOP{ holeCardsOOP }, mHoleCardsIP{ holeCardsIP },
			mPot{ prevPot }, mIsAllIn{ isAllIn } {}


		Board* GetDrawCards(Solver* staticGameInfo);
		static float DrawUniformProb(Board* drawCards);
		bool IsAllIn() const { return mIsAllIn; }

		std::string ToHash() const { return Utils::NO_CARD; }

		static void HandleDealingHandOOP(std::vector<ClientNode<Bet, PokerPlayerNode, ChanceNode>>& list, const ChanceNode& p, Solver* g);
		static void HandleDealingHandIP(std::vector<ClientNode<Bet, PokerPlayerNode, ChanceNode>>& list, const ChanceNode& p, Solver* g);
		static void HandleFlopAllIn(std::vector<ClientNode<Bet, PokerPlayerNode, ChanceNode>>& list, const ChanceNode& p, Solver* g);
		static void HandleTurnAllIn(std::vector<ClientNode<Bet, PokerPlayerNode, ChanceNode>>& list, const ChanceNode& p, Solver* g);
		static void HandleRiverAllIn(std::vector<ClientNode<Bet, PokerPlayerNode, ChanceNode>>& list, const ChanceNode& p, Solver* g);
		static void HandleTurnDraw(std::vector<ClientNode<Bet, PokerPlayerNode, ChanceNode>>& vector, const ChanceNode& chance_node, Solver* solver);
		static void HandleRiverDraw(std::vector<ClientNode<Bet, PokerPlayerNode, ChanceNode>>& list, const ChanceNode& p, Solver* g);

		std::vector<ClientNode<Bet, PokerPlayerNode, ChanceNode>> Children(Solver* staticInfo) const;
		

	};

	typedef TreeNode<Bet, PokerPlayerNode, ChanceNode> HistoryNode;

	float UtilityFunc(std::vector<HistoryNode> const history);

	

};




template <typename Elem>
class HoleCardMatrix {

	typedef std::vector<Elem*> MatrixList;
public:
	Solver::Range* mRangeOOP;
	Solver::Range* mRangeIP;

	int mRangeIpSize;
	MatrixList mMatrix;


	HoleCardMatrix() :
		mRangeOOP{ nullptr }, mRangeIP{ nullptr },
		mRangeIpSize{ 0 }, mMatrix{} {}

	HoleCardMatrix(Solver::Range* rangeOOP, Solver::Range* rangeIP) {
		int totalSize = (rangeOOP->size() + 1) * (rangeIP->size() + 1);
		mMatrix = MatrixList(totalSize, nullptr);
		mRangeIpSize = rangeIP->size();
		mRangeOOP = rangeOOP;
		mRangeIP = rangeIP;
	}

	
	void SetElem(Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP, Elem* pElem) {
		int iOOP = CardsOOP.size() != 4? 0 : mRangeOOP->HandIndex(CardsOOP) + 1;
		int iIP = CardsIP.size() != 4 ? 0 : mRangeIP->HandIndex(CardsIP) + 1;
		int tableIndex = ( iOOP * (mRangeIpSize + 1) ) + iIP;
		mMatrix.at(tableIndex) = pElem;
	}

	
	auto* GetElem(Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP) {
		int iOOP = CardsOOP.size() != 4 ? 0 : mRangeOOP->HandIndex(CardsOOP) + 1;
		int iIP = CardsIP.size() != 4 ? 0 : mRangeIP->HandIndex(CardsIP) + 1;
		int tableIndex = ( iOOP * (mRangeIpSize  + 1)) + iIP;
		return mMatrix.at(tableIndex);
	}
};



template <typename Elem>
class CardMatrix {

	typedef std::vector<Elem*> MatrixList;

	MatrixList mMatrix;

public:
	CardMatrix() {
		long totalSize = ( Utils::DECK_SIZE + 1 ) * ( Utils::DECK_SIZE + 1 );
		mMatrix = MatrixList(totalSize, nullptr);
	}


	void SetElem(const std::string& turnCard, const std::string& riverCard, Elem* pElem) {

		int iTurn = turnCard.size() != 2 ? 0 : Utils::CardIndex(turnCard) + 1;
		int iRiver = riverCard.size() != 2 ? 0 : Utils::CardIndex(riverCard) + 1;
		int tableIndex = ( iTurn * Utils::DECK_SIZE + 1 ) + iRiver;
		mMatrix.at(tableIndex) = static_cast<Elem*>(pElem);
	}

	auto* GetElem(std::string& turnCard, std::string& riverCard) {
		int iTurn = turnCard.size() != 2 ? 0 : Utils::CardIndex(turnCard) + 1;
		int iRiver = riverCard.size() != 2 ? 0 : Utils::CardIndex(riverCard) + 1;
		int tableIndex = ( iTurn * Utils::DECK_SIZE + 1 ) + iRiver;
		return static_cast<Elem*>(mMatrix.at(tableIndex));
	}
};


class DrawCardsTable : public HoleCardMatrix<typename CardMatrix<Solver::Board>> {

public:
	DrawCardsTable(Solver::Range* rangeOOP, Solver::Range* rangeIP, Solver::Board& flop);

	Solver::Board* GetCardsToDraw(
		Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP,
		Solver::Card& turnCard
	);

};

class WinnerTable : public HoleCardMatrix<typename CardMatrix<int8_t>> {

public:
	WinnerTable(Solver::Range* rangeOOP, Solver::Range* rangeIP,
				Solver::Board& flop, DrawCardsTable* drawCardTable);

	int GetShowdownWinner(
		Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP,
		Solver::Card& turnCard, Solver::Card& riverCard
	);

};
