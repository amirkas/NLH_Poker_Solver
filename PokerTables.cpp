#include "pch.h"
#include "Solver.h"
#include "PokerUtils.h"
#include "EvaluatorLibrary.h"


DrawCardsTable::DrawCardsTable(Solver::Range* rangeOOP, Solver::Range* rangeIP, Solver::Board& flop)
	: HoleCardMatrix<CardMatrix<Solver::Board>>{rangeOOP, rangeIP}
{



	auto iHoleCardOOP = rangeOOP->begin();
	auto iEndOOP = rangeOOP->end();

	for (iHoleCardOOP; iHoleCardOOP < iEndOOP; iHoleCardOOP++) {

		std::string CardStrOOP = *iHoleCardOOP;
		auto iHoleCardIP = rangeIP->begin();
		auto iEndIP = rangeIP->end();
		

		for (iHoleCardIP; iHoleCardIP < iEndIP; iHoleCardIP++) {
			
			std::string CardStrIP = *iHoleCardIP;
			std::string CombinedCardStr = CardStrOOP + CardStrIP + flop;

			//Create new CardToElem Matrix for each combination of hole cards.
			CardMatrix<Solver::Board> * drawTable = new CardMatrix<Solver::Board>();

			//Add empty turn for potential turn draw cards.
			Solver::Board *turnCards = new std::string(Utils::RemainingCards(CombinedCardStr));
			drawTable->SetElem(Utils::NO_CARD, Utils::NO_CARD, turnCards);

			//For each card in turn cards, find potential river cards.
			for (int iTurnCard = 0; iTurnCard < turnCards->size(); iTurnCard += 2) {
				
				std::string turnCard = turnCards->substr(iTurnCard, 2);
				std::string turnBoard = CombinedCardStr + turnCard;
				Solver::Board *riverCards = new std::string(Utils::RemainingCards(turnBoard));
				drawTable->SetElem(turnCard, Utils::NO_CARD, riverCards);
			}
			//Add draw Table to HoleCardsMatrix for current combination of hole cards.
			SetElem(CardStrOOP, CardStrIP, drawTable);
		}
	}
}

Solver::Board* DrawCardsTable::GetCardsToDraw(
	Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP, Solver::Card& turnCard
) {
	CardMatrix<Solver::Board>* possibleCardMatrix = GetElem(CardsOOP, CardsIP);
	return possibleCardMatrix->GetElem(turnCard, Utils::NO_CARD);
}


WinnerTable::WinnerTable(
	Solver::Range* rangeOOP, Solver::Range* rangeIP,
	Solver::Board& flop, DrawCardsTable* drawCardTable
) : HoleCardMatrix<CardMatrix<int8_t>>{ rangeOOP, rangeIP }
{
	auto iHoleCardOOP = rangeOOP->begin();
	auto iEndOOP = rangeOOP->end();

	for (iHoleCardOOP; iHoleCardOOP < iEndOOP; iHoleCardOOP++) {

		std::string CardStrOOP = *iHoleCardOOP;
		auto iHoleCardIP = rangeIP->begin();
		auto iEndIP = rangeIP->end();

		for (iHoleCardIP; iHoleCardIP < iEndIP; iHoleCardIP++) {

			std::string CardStrIP = *iHoleCardIP;
			std::string CombinedCardStr = CardStrOOP + CardStrIP;

			Solver::Board* turnCards = drawCardTable->GetCardsToDraw(CardStrOOP, CardStrIP, Utils::NO_CARD);

			//Create new Card To uint8_t matrix to store winner.
			CardMatrix<int8_t> *winnerMatrix = new CardMatrix<int8_t>();

			//Get every possible combination of river cards.
			for (int iTurnCard = 0; iTurnCard < turnCards->size(); iTurnCard += 2) {

				std::string turnCard = turnCards->substr(iTurnCard, 2);
				Solver::Board* riverCards = drawCardTable->GetCardsToDraw(CardStrOOP, CardStrIP, turnCard);

				for (int iRiverCard = 0; iRiverCard < riverCards->size(); iRiverCard += 2) {

					std::string riverCard = riverCards->substr(iRiverCard, 2);

					std::string completeBoard = flop + turnCard + riverCard;
					int8_t* winner = new int8_t(Get_Winner(CardStrOOP, CardStrIP, completeBoard));
					winnerMatrix->SetElem(turnCard, riverCard, winner);
				}	
			}
			//Add winner matrix pointer to Hole Card matrix.
			SetElem(CardStrOOP, CardStrIP, winnerMatrix);
		}
	}
}

int WinnerTable::GetShowdownWinner(
	Solver::HoleCards& CardsOOP, Solver::HoleCards& CardsIP,
	Solver::Card& turnCard, Solver::Card& riverCard
) {
	CardMatrix<int8_t>* winnerMatrix = GetElem(CardsOOP, CardsIP);
	return (int) *(winnerMatrix->GetElem(turnCard, riverCard));
}



