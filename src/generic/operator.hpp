#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include "utils.hpp"
#include "solution.hpp"

using uint = unsigned int;
using permutator::fitness_t;
using std::vector;



namespace oprtr
{
void insert(std::vector<uint> & permutation, std::vector<uint> & frequency, std::vector<uint> upperBounds, uint node, uint position);

void append(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> upperBounds, uint node);

void remove(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lowerBounds, uint position);

void relocate(std::vector<uint> &permutation,  uint from, uint to, uint length, bool reverse);

void centeredExchange(std::vector<uint> &permutation,  uint center, uint radius);

void exchange(std::vector<uint> &permutation,  uint from, uint to, uint sizeX, uint sizeY, bool exchange);

void moveAll(std::vector<uint> &permutation, uint node, int offset, std::vector<uint>* position=nullptr);

void exchangeIds(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lowerBounds, std::vector<uint> upperBounds, uint nodeX, uint nodeY);

void exchangeNIds(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lowerBounds, std::vector<uint> upperBounds, uint nodeX, uint nodeY, uint maxSwap);

void reverse(std::vector<uint> &permutation, uint position, uint length);

}

namespace construct
{
// void greedy(Solution& initSolution, std::function<fitness_t(bool<uint>)> inBoundsFunc, std::function<fitness_t(std::vector<uint>)> fitnessFunc);

// void nearestNeighbor(Solution& initSolution, std::function<fitness_t(bool<uint>)> inBoundsFunc, std::function<fitness_t(std::vector<uint>)> fitnessFunc);

void random(std::vector<uint> &permutation, std::vector<uint> &frequency, vector<uint> lbs);

void randomReplicate(std::vector<uint> &permutation, std::vector<uint> &frequency, vector<uint> lbs, vector<uint> ubs);

}