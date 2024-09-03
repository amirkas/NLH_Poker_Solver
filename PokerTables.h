//#pragma once
//#include <string>
//#include "PokerUtils.h"
//#include "Solver.h"
//#include "EvaluatorLibrary.h"
//
///**
// * @brief Forward declarations
// */
//class Solver;
//
//
//template <typename Elem> 
//class HoleCardMatrix {
//
//	typedef std::vector<Elem*> MatrixList;
//
//	Solver::Range* mRangeOOP;
//	Solver::Range* mRangeIP;
//
//	int mRangeIpSize;
//	MatrixList mMatrix;
//
//public:
//	HoleCardMatrix() :
//		mRangeOOP{ nullptr }, mRangeIP{ nullptr },
//		mRangeIpSize{ 0 }, mMatrix{} {}
//
//	HoleCardMatrix(Solver::Range* rangeOOP, Solver::Range* rangeIP) {
//		int totalSize = rangeOOP->size() * rangeIP->size();
//		mMatrix(totalSize, nullptr);
//		mRangeIPSize = rangeIP->size();
//		mRangeOOP = rangeOOP;
//		mRangeIP = rangeIP;
//	}
//
//	void SetElem(Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP, Elem* pElem) {
//		int iOOP = mRangeOOP->HandIndex(CardsOOP);
//		int iIP  = mRangeIP->HandIndex(CardsIP);
//		int tableIndex = (iOOP * mRangeIpSize) + iIP;
//		mMatrix.at(tableIndex) = pElem;
//	}
//
//	Elem* GetElem(Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP) {
//		int iOOP = mRangeOOP->HandIndex(CardsOOP);
//		int iIP = mRangeIP->HandIndex(CardsIP);
//		int tableIndex = ( iOOP * mRangeIpSize ) + iIP;
//		return mMatrix.at(tableIndex);
//	}
//};
//
//template <typename Elem>
//class CardMatrix {
//
//	typedef std::vector<Elem*> MatrixList;
//
//	MatrixList mMatrix;
//
//public:
//	CardMatrix(){
//		long totalSize = (Utils::DECK_SIZE + 1) * ( Utils::DECK_SIZE + 1 );
//		mMatrix(totalSize, nullptr);
//	}
//
//
//	void SetElem(const Solver::Card& turnCard, const Solver::Card& riverCard, Elem* pElem) {
//		
//		int iTurn = turnCard.size() != 2 ? 0 : Utils::CardIndex(turnCard) + 1;
//		int iRiver = riverCard.size() != 2 ? 0 : Utils::CardIndex(riverCard) + 1;
//		int tableIndex = (iTurn * Utils::DECK_SIZE + 1) + iRiver;
//		mMatrix.at(tableIndex) = pElem;
//	}
//
//	Elem* GetElem(Solver::Card& turnCard, Solver::Card& riverCard) {
//		int iTurn = turnCard.size() != 2 ? 0 : Utils::CardIndex(turnCard) + 1;
//		int iRiver = riverCard.size() != 2 ? 0 : Utils::CardIndex(riverCard) + 1;
//		int tableIndex = ( iTurn * Utils::DECK_SIZE + 1 ) + iRiver;
//		return mMatrix.at(tableIndex);
//	}
//};
//
//
//class DrawCardsTable : public HoleCardMatrix<CardMatrix<Solver::Board>> {
//
//public:
//	DrawCardsTable(Solver::Range* rangeOOP, Solver::Range* rangeIP, Solver::Board& flop);
//
//	Solver::Board* GetCardsToDraw(
//		Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP,
//		Solver::Card& turnCard
//	);
//
//};
//
//class WinnerTable : public HoleCardMatrix<CardMatrix<uint8_t>> {
//
//public:
//	WinnerTable(Solver::Range* rangeOOP, Solver::Range* rangeIP,
//				Solver::Board& flop, DrawCardsTable* drawCardTable);
//
//	int GetShowdownWinner(
//		Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP,
//		Solver::Card& turnCard, Solver::Card& riverCard
//	);
//
//};














