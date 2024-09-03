#pragma once
#include <string>
#include <unordered_map>

class Utils {
public:
	static std::string RANKS;
	static std::string SUITS;
	static std::string NO_CARD;

	static const int RANK_SIZE = 13;
	static const int SUIT_SIZE = 4;
	static const int DECK_SIZE = RANK_SIZE * SUIT_SIZE;

	static std::unordered_map<char, int> RanksToInt;
	static std::unordered_map<char, int> SuitsToInt;

	static int RankToInt(const std::string& card);

	static int SuitToInt(const std::string& suit);

	static int CardIndex(const std::string& card);

	static int CardComparator(const std::string& cardOne, const std::string& cardTwo);

	static std::string SortTwoCards(std::string cardOne, std::string cardTwo);

	static std::string RemainingCards(const std::string& seenCards);

	static std::string CardFromBoard(const std::string& board, int index);

};