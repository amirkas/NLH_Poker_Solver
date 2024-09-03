#pragma once
#include <string>
#include <utility>

#ifdef EVALUATOR_EXPORTS
#define NLH_SOLVER_API __declspec(dllexport)
#else
#define NLH_SOLVER_API __declspec(dllexport)
#endif

enum Street {
	FLOP, TURN, RIVER
};

enum Player {
	OOP, IP
};




extern "C" NLH_SOLVER_API void SetMaxBetDepth(int maxDepth);

extern "C" NLH_SOLVER_API void AddBetToStrategy(Player player, Street street, int betDepth, std::string bet);

extern "C" NLH_SOLVER_API void AddHandToRange(Player player, std::string holeCards);

extern "C" NLH_SOLVER_API void InitSolver(std::string flop, float startPot, float effStack);

extern "C" NLH_SOLVER_API void RunSolve();

extern "C" NLH_SOLVER_API long TreeSize();

extern "C" NLH_SOLVER_API void PrintSolve();


