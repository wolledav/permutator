#pragma once

#include <random>
#include <iterator>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <numeric>
#include <functional>
#include <stack>

#include "utils.hpp"

namespace oprtr
{
void insert(std::vector<uint> & permutation, std::vector<uint> & frequency, std::vector<uint> ubs, uint node, uint position);

void append(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> ubs, uint node);

void remove(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lbs, uint position);

void relocate(std::vector<uint> &permutation,  uint from, uint to, uint length, bool reverse);

void centeredExchange(std::vector<uint> &permutation,  uint center, uint radius);

void exchange(std::vector<uint> &permutation,  uint from, uint to, uint sizeX, uint sizeY, bool reverse);

void moveAll(std::vector<uint> &permutation, uint node, int offset, std::vector<uint>* position=nullptr);

void exchangeIds(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lbs, std::vector<uint> ubs, uint nodeX, uint nodeY);

void exchangeNIds(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lbs, std::vector<uint> ubs, uint nodeX, uint nodeY, uint maxSwap);

void reverse(std::vector<uint> &permutation, uint position, uint length);

}

namespace construct
{
void random(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lbs, std::mt19937 *rng);

void randomReplicate(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lbs, std::vector<uint> ubs, std::mt19937 *rng);

}


namespace crossover
{
    void ERX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::vector<uint> lbs, std::vector<uint> ubs, std::mt19937 *rng);

    void AEX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::vector<uint> lbs, std::vector<uint> ubs, std::mt19937 *rng);

    void NBX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(std::vector<uint> & x, std::vector<uint> & y, uint gap_node)> alignmentFunction);

    void PBX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(std::vector<uint> & x, std::vector<uint> & y, uint gap_node)> alignmentFunction);

    void OX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(std::vector<uint> & x, std::vector<uint> & y, uint gap_node)> alignmentFunction);

    void OBX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::vector<uint> freq1, std::vector<uint>freq2, std::mt19937 *rng);

    void CX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(std::vector<uint> & x, std::vector<uint> & y, uint gap_node)> alignmentFunction);

    void HXHelper(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::vector<uint> lbs, std::vector<uint> ubs, std::function<uint(std::vector<uint>, std::vector<uint>)> getNextNode, std::mt19937 *rng);

    void HGreX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::vector<uint> lbs, std::vector<uint> ubs, std::function<permutator::fitness_t(std::vector<uint>)> getFitness, std::mt19937 *rng);

    void HRndX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::vector<uint> lbs, std::vector<uint> ubs, std::function<permutator::fitness_t(std::vector<uint>)> getFitness, std::mt19937 *rng);
    
    void HProX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::vector<uint> lbs, std::vector<uint> ubs, std::function<permutator::fitness_t(std::vector<uint>)> getFitness, std::mt19937 *rng);

    void rULX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::vector<uint> lbs, std::vector<uint> ubs,  bool random, std::mt19937 *rng, std::function<void(std::vector<uint> & x, std::vector<uint> & y, uint gap_node)> alignmentFunction);

    void EULX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::function<permutator::fitness_t(std::vector<uint>)> getFitness);

    void UPMX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(std::vector<uint> & x, std::vector<uint> & y, uint gap_node)> alignmentFunction);

    void SPX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, uint node_cnt, std::function<permutator::fitness_t(std::vector<uint>)> getFitness, std::mt19937 *rng, std::function<void(std::vector<uint> & x, std::vector<uint> & y, uint gap_node)> alignmentFunction);



    // void PMX(std::vector<uint> parent1, std::vector<uint> parent2, std::vector<uint> &child, std::vector<uint> freq1, std::vector<uint> freq2, std::mt19937 *rng);
}

namespace alignment
{
    void globalUniform(std::vector<uint> &x, std::vector<uint> &y, uint gap_node, uint difference_penalty, uint gap_penalty);

    void globalOneGap(std::vector<uint> &x, std::vector<uint> &y, uint gap_node);

    void randomOneGap(std::vector<uint> &x, std::vector<uint> &y, uint gap_node, std::mt19937 *rng);

    void randomUniform(std::vector<uint> &x, std::vector<uint> &y, uint gap_node, std::mt19937 *rng);

    void backfillOneGap(std::vector<uint> &x, std::vector<uint> &y, uint gap_node);

    void frontfillOneGap(std::vector<uint> &x, std::vector<uint> &y, uint gap_node);

    void removeGap(std::vector<uint> &x, uint gap_node);

}
