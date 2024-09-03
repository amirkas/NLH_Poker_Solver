#include "pch.h"
#include "PokerUtils.h"
#include <algorithm>


std::string Utils::RANKS = "23456789TJQKA";
std::string Utils::SUITS = "dchs";
std::string Utils::NO_CARD = "";

std::unordered_map<char, int> Utils::RanksToInt =
{
	{'A', 12}, {'K', 11}, {'Q', 10}, {'J', 9}, {'T', 8}, {'9', 7},
	{'8', 6}, {'7', 5}, {'6', 4}, {'5', 3}, {'4', 2}, {'3', 1}, {'2', 0},
};

std::unordered_map<char, int> Utils::SuitsToInt =
{
	{'s', 3}, {'h', 2}, {'c', 1}, {'d', 0}
};

int Utils::RankToInt(const std::string& card) {
	char rankChar = card[0];
	return RanksToInt.at(rankChar);
}

int Utils::SuitToInt(const std::string& card) {
	char suitChar = card[1];
	return SuitsToInt.at(suitChar);
}

int Utils::CardIndex(const std::string& card) {
	if (card.size() == 0) {
		return -1;
	}
	int suitIndex = SuitToInt(card);
	int rankIndex = RankToInt(card);
	return ( suitIndex * Utils::RANK_SIZE ) + rankIndex;
}

int Utils::CardComparator(const std::string& cardOne, const std::string& cardTwo) {
	int rankOne = Utils::RankToInt(cardOne);
	int rankTwo = Utils::RankToInt(cardTwo);

	//Checking if card is empty.
	if (cardOne.size() == 0 && cardTwo.size() == 0) {
		return 0;
	}
	else if (cardOne.size() == 0) {
		return -1;
	}
	else if (cardTwo.size() == 0) {
		return 1;
	}

	if (rankOne > rankTwo) {
		return 1;
	}
	else if (rankOne < rankTwo) {
		return -1;
	}
	else {
		int suitOne = Utils::SuitToInt(cardOne);
		int suitTwo = Utils::SuitToInt(cardTwo);
		if (suitOne > suitTwo) {
			return 1;
		}
		else if (suitOne < suitTwo) {
			return -1;
		}
		return 0;
	}
}

std::string Utils::SortTwoCards(std::string cardOne, std::string cardTwo) {
	int comparison = CardComparator(cardOne, cardTwo);
	if (comparison >= 0) {
		return cardOne + cardTwo;
	}
	return cardTwo + cardOne;
}

std::string Utils::RemainingCards(const std::string& seenCards) {

	std::vector<int> seen;
	for (int iCard = 0; iCard < seenCards.size(); iCard += 2) {
		std::string card = seenCards.substr(iCard, 2);
		int cardIndex = CardIndex(card);
		seen.push_back(cardIndex);
	}
	std::string remaining("aa", 2 * (DECK_SIZE - seen.size() ));
	int iRemaining = 0;
	for (int iSuit = 0; iSuit < SUIT_SIZE; iSuit++) {
		for (int iRank = 0; iRank < RANK_SIZE; iRank++) {
			
			int boardCardIndex = (iSuit * RANK_SIZE) + iRank;
			if (std::find(seen.begin(), seen.end(), boardCardIndex) != seen.end()) {
				continue;
			}
			else {
				remaining[iRemaining] = RANKS[iRank];
				remaining[iRemaining + 1] = SUITS[iSuit];
				iRemaining += 2;
			}
		}
	}
	return remaining;
}

std::string Utils::CardFromBoard(const std::string& board, int index) {
	if (index * 2 > board.size()) {
		return "";
	}
	return board.substr(index * 2, 2);
}

