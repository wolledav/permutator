#pragma once

#include <vector>
#include <algorithm>
#include "utils.hpp"

using uint = unsigned int;

namespace oprtr
{

void insert(std::vector<uint> & permutation, std::vector<uint> & frequency, std::vector<uint> upperBounds, uint node, uint position);

void append(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> upperBounds, uint node);

void remove(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lowerBounds, uint position);

void relocate(std::vector<uint> &permutation,  uint from, uint to, uint length, bool reverse);

void centeredExchange(std::vector<uint> &permutation,  uint center, uint radius);

void exchange(std::vector<uint> &permutation,  uint from, uint to, uint sizeX, uint sizeY, bool exchange);

void moveAll(std::vector<uint> &permutation, uint node, uint offset, std::vector<uint>* position=nullptr);

void exchangeIds(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lowerBounds, std::vector<uint> upperBounds, uint nodeX, uint nodeY);

void exchangeNIds(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lowerBounds, std::vector<uint> upperBounds, uint nodeX, uint nodeY, uint maxSwap);

void twoOpt(std::vector<uint> &permutation, uint position, uint length);

}