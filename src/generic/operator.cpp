
#include "operator.hpp"

using std::vector;
using std::queue;

// inserts 'node' at 'position'
void oprtr::insert(vector<uint> &permutation, vector<uint> &frequency, vector<uint> upperBounds, uint node, uint position)
{
    if (frequency[node] < upperBounds[node] && position <= permutation.size())
    {
        permutation.insert(permutation.begin() + position, node);
        frequency[node]++;
    }
}

// appends 'node' at the end of 'permutation'
void oprtr::append(vector<uint> &permutation, vector<uint> &frequency, vector<uint> upperBounds, uint node)
{
    if (frequency[node] < upperBounds[node])
    {
        permutation.emplace_back(node);
        frequency[node]++;
    }
}

// removes 'node' at 'position'
void oprtr::remove(vector<uint> &permutation, vector<uint> &frequency, vector<uint> lowerBounds, uint position)
{
    if (position < permutation.size() && frequency[permutation[position]] > lowerBounds[permutation[position]])
    {
        frequency[permutation[position]]--;
        permutation.erase(permutation.begin() + position);
    }
}

// moves subsequence starting at 'from' of length 'length' to 'to', reverses subsequence if reverse==true
// relocate([1,2,3,4], 1, 2, 2, false) -> [1,4,2,3]
void oprtr::relocate(vector<uint> &permutation, uint from, uint to, uint length, bool reverse)
{
    if (from + length > permutation.size() || to > permutation.size() - length)
        return;
    vector<uint> subsequence(permutation.begin() + from, permutation.begin() + from + length);
    permutation.erase(permutation.begin() + from, permutation.begin() + from + length);
    if (reverse)
    {
        permutation.insert(permutation.begin() + to, subsequence.rbegin(), subsequence.rend());
    }
    else
    {
        permutation.insert(permutation.begin() + to, subsequence.begin(), subsequence.end());
    }
}

// swaps nodes 'radius' distance from 'center'
// centeredExchange([1,2,3,4], 2, 1) -> [1,4,3,2]
void oprtr::centeredExchange(vector<uint> &permutation, uint center, uint radius)
{

    uint position = center - radius;
    relocate(permutation, position, position, 2 * radius + 1, true);
}

// swaps subsequences at 'posX' and 'posY', lengths of subsequences are 'sizeX' and 'sizeY', reverses subsequences if reverse==true
void oprtr::exchange(vector<uint> &permutation, uint posX, uint posY, uint sizeX, uint sizeY, bool reverse)
{

    if ((posX >= posY && posX < posY + sizeY) || (posY >= posX && posY < posX + sizeX))
        return;

    vector<uint> subx, suby;
    if (reverse)
    {
        subx.insert(subx.begin(), permutation.rend() - posX - sizeX, permutation.rend() - posX);
        suby.insert(suby.begin(), permutation.rend() - posY - sizeY, permutation.rend() - posY);
    }
    else
    {
        subx.insert(subx.begin(), permutation.begin() + posX, permutation.begin() + posX + sizeX);
        suby.insert(suby.begin(), permutation.begin() + posY, permutation.begin() + posY + sizeY);
    }
    if (posX < posY)
    {
        permutation.erase(permutation.begin() + posY, permutation.begin() + posY + sizeY);
        permutation.insert(permutation.begin() + posY, subx.begin(), subx.end());
        permutation.erase(permutation.begin() + posX, permutation.begin() + posX + sizeX);
        permutation.insert(permutation.begin() + posX, suby.begin(), suby.end());
    }
    else
    {
        permutation.erase(permutation.begin() + posX, permutation.begin() + posX + sizeX);
        permutation.insert(permutation.begin() + posX, suby.begin(), suby.end());
        permutation.erase(permutation.begin() + posY, permutation.begin() + posY + sizeY);
        permutation.insert(permutation.begin() + posY, subx.begin(), subx.end());
    }
}

// moves all occurrences of 'node' by 'offset', positions of nodes may by predefined beforehand (unexpected behavior if nodes are not in predefined positions)
void oprtr::moveAll(vector<uint> &permutation, uint node, int offset, vector<uint> *positions)
{
    if (offset == 0)
        return;

    vector<uint> *new_positions = positions == nullptr ? new vector<uint>(0) : positions;
    if (positions == nullptr)
    {
        for (int i = permutation.size() - 1; i >= 0; i--)
        {
            if (permutation[i] == node)
                positions->push_back(i);
        }
    }

    std::sort(positions->begin(), positions->end(), std::greater<>());

    for (auto &pos : *positions)
    {                                    // for all positions of node_id
        int new_pos = (int)pos + offset; // shift by i
        if (new_pos < 0)
            new_pos += (int)permutation.size(); // too far left -> overflow from right end
        else if ((uint)new_pos >= permutation.size())
            new_pos = new_pos - (int)permutation.size(); // too far right -> overflow from left end
        permutation.erase(permutation.begin() + pos);
        pos = new_pos;
    }
    std::sort(positions->begin(), positions->end());
    for (auto pos : *positions)
        permutation.insert(permutation.begin() + pos, node);
}

// swaps all occurrences of 'nodeX' and 'nodeY', if frequencies are in bounds
// exchangeIds([1,2,3,1,2,1], freq, lbs, ubs, 1, 2) -> [2,1,3,2,1,2]
void oprtr::exchangeIds(vector<uint> &permutation, vector<uint> &frequency, vector<uint> lowerBounds, vector<uint> upperBounds, uint nodeX, uint nodeY)
{
    if (frequency[nodeX] < lowerBounds[nodeY] || frequency[nodeY] < lowerBounds[nodeX] ||
        frequency[nodeX] > upperBounds[nodeY] || frequency[nodeY] > upperBounds[nodeX])
        return;

    for (unsigned int &node : permutation)
    {
        if (node == nodeX)
        {
            node = nodeY;
            frequency[nodeX]--;
            frequency[nodeY]++;
        }
        else if (node == nodeY)
        {
            node = nodeX;
            frequency[nodeY]--;
            frequency[nodeX]++;
        }
    }
}
// swaps first 'maxSwap' occurrences of 'nodeX' and 'nodeY', if frequencies are in bounds
// exchangeIds([1,2,3,1,2,1], freq, lbs, ubs, 1, 2, 2) -> [2,1,3,2,1,1]
void oprtr::exchangeNIds(vector<uint> &permutation, vector<uint> &frequency, vector<uint> lowerBounds, vector<uint> upperBounds, uint nodeX, uint nodeY, uint maxSwap)
{
    if (frequency[nodeX] < lowerBounds[nodeY] || frequency[nodeY] < lowerBounds[nodeX] ||
        maxSwap < lowerBounds[nodeX] || maxSwap < lowerBounds[nodeY] ||
        (frequency[nodeX] > upperBounds[nodeY] && maxSwap > upperBounds[nodeY]) ||
        (frequency[nodeY] > upperBounds[nodeX] && maxSwap > upperBounds[nodeX]))
        return;
    uint counter1 = 0, counter2 = 0;
    vector<uint> new_perm = permutation;
    for (unsigned int &node : new_perm)
    { // for all nodes in new_perm
        if (node == nodeX && counter1 < maxSwap)
        { // if i swapped less than n times
            counter1++;
            node = nodeY;
            frequency[nodeX]--;
            frequency[nodeY]++;
        }
        else if (node == nodeY && counter2 < maxSwap)
        { // if j swapped less than n times
            counter2++;
            node = nodeX;
            frequency[nodeY]--;
            frequency[nodeX]++;
        }
    }
}

// reverses subsequence starting at 'position' of length 'length'
void oprtr::reverse(vector<uint> &permutation, uint position, uint length)
{
    if (position + length > permutation.size())
        return;
    reverse(permutation.begin() + position, permutation.begin() + position + length);
}

void construct::random(vector<uint> &permutation, vector<uint> &frequency, vector<uint> lbs, std::mt19937 *rng)
{

    uint rnd_idx;
    uint node_cnt = frequency.size();
    // initSolution.print();
    for (uint node_id = 0; node_id < node_cnt; node_id++)
    { // for all nodes
        while (frequency[node_id] < lbs[node_id])
        {
            std::uniform_int_distribution<uint> uni(0, permutation.size());
            rnd_idx = uni(*rng);
            permutation.insert(permutation.begin() + rnd_idx, node_id);
            frequency[node_id]++;
        }
    }
}

void construct::randomReplicate(vector<uint> &permutation, vector<uint> &frequency, vector<uint> lbs, vector<uint> ubs, std::mt19937 *rng)
{

    uint node_cnt = frequency.size();

    for (auto node : permutation)
    {
        if (frequency[node] == 0)
        {
            permutation.push_back(node);
            frequency[node]++;
        }
    }
    auto offset = permutation.size();

    // Append missing nodes
    for (uint i = 0; i < node_cnt; i++)
    {
        if (frequency[i] == 0)
        {
            permutation.push_back(i);
            frequency[i]++;
        }
    }

    // Shuffle missing nodes
    auto begin = permutation.begin();
    std::advance(begin, offset);
    std::shuffle(begin, permutation.end(), *rng);

    // Replicate, until all nodes are within bounds

    // initSolution = Solution(node_cnt);

    auto inBounds = [lbs, ubs](vector<uint> freq)
    {
        for (uint i = 0; i < freq.size(); i++)
            if (freq[i] < lbs[i] || freq[i] > ubs[i])
                return false;
        return true;
    };

    auto perm = permutation;
    do
    {
        for (auto elem : perm)
        {
            if (frequency[elem] < lbs[elem])
            {
                frequency[elem]++;
                permutation.push_back(elem);
            }
        }
    } while (!inBounds(frequency));
}

void crossover::ERX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::mt19937 *rng)
{
    child.clear();
    uint node_cnt = lbs.size();
    vector<uint> freq(node_cnt, 0);

    std::unordered_map<uint, vector<uint>> neighborMap;
    for (uint node = 0; node < node_cnt; node++)
        neighborMap[node] = {};

    for (auto parent : {parent1, parent2})
    {
        uint size = parent.size();
        for (uint i = 0; i < size; i++)
        {
            uint node = parent[i];
            uint next = parent[i + 1 == size ? 0 : i + 1]; // wrap around permutation
            uint prev = parent[i == 0 ? size - 1 : i - 1]; // wrap around permutation
            neighborMap.at(node).push_back(next);
            neighborMap.at(node).push_back(prev);
        }
    }

    child.push_back(parent1[0]);
    freq[parent1[0]] += 1;

    while (true)
    {
        uint current = child.back();
        vector<uint> neighbors = neighborMap.at(current);
        vector<uint> possibleNext(0);

        // check if any neighbor can be inserted
        for (auto node : neighbors)
            if (freq[node] < ubs[node])
                possibleNext.push_back(node);
        // if not insert node bellow lbs
        if (possibleNext.empty())
            for (uint node = 0; node < node_cnt; node++)
                if (freq[node] < lbs[node])
                    possibleNext.push_back(node);
        // no node bellow lbs -> can break
        if (possibleNext.size() == 0)
            break;

        std::uniform_int_distribution<uint> randNode(0, possibleNext.size() - 1);
        uint next = possibleNext[randNode(*rng)];
        child.push_back(next);
        freq[next] += 1;

        // erase edge from current and next
        auto it = find(neighborMap.at(current).begin(), neighborMap.at(current).end(), next);
        if (it != neighborMap.at(current).end())
        {
            neighborMap.at(current).erase(it);
            it = find(neighborMap.at(next).begin(), neighborMap.at(next).end(), current);
            neighborMap.at(next).erase(it);
        }
    }
}

void crossover::AEX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::mt19937 *rng)
{
    child.clear();
    uint node_cnt = lbs.size();
    vector<uint> freq(node_cnt, 0);

    std::unordered_map<uint, vector<uint>> parent1Map;
    std::unordered_map<uint, vector<uint>> parent2Map;
    for (uint node = 0; node < node_cnt; node++)
    {
        parent1Map[node] = {};
        parent2Map[node] = {};
    }

    for (uint i = 0; i < parent1.size() - 1; i++)
        parent1Map.at(parent1[i]).push_back(parent1[i + 1]);
    for (uint i = 0; i < parent2.size() - 1; i++)
        parent2Map.at(parent2[i]).push_back(parent2[i + 1]);

    uint temp = 0;
    std::unordered_map<uint, vector<uint>> *neighborMap;
    child.push_back(parent1[0]);
    freq[parent1[0]] += 1;

    while (true)
    {
        neighborMap = temp++ % 2 == 0 ? &parent1Map : &parent2Map;
        uint current = child.back();
        vector<uint> neighbors = neighborMap->at(current);
        vector<uint> possibleNext(0);

        // check if any neighbor can be inserted
        for (auto node : neighbors)
            // first feasible node is selected
            if (freq[node] < ubs[node])
            {
                possibleNext.push_back(node);
                break;
            }
        // if not insert node bellow lbs
        if (possibleNext.empty())
            for (uint node = 0; node < node_cnt; node++)
                if (freq[node] < lbs[node])
                    possibleNext.push_back(node);

        uint next;
        // no node bellow lbs -> can break
        if (possibleNext.size() == 0)
        {
            break;
        }
        // first feasible node is selected
        else if (possibleNext.size() == 1)
        {
            next = possibleNext[0];
            // erase edge from current
            auto it = find(neighborMap->at(current).begin(), neighborMap->at(current).end(), next);
            if (it != neighborMap->at(current).end())
                neighborMap->at(current).erase(it);
        }
        else
        {
            std::uniform_int_distribution<uint> randNode(0, possibleNext.size() - 1);
            next = possibleNext[randNode(*rng)];
        }
        child.push_back(next);
        freq[next] += 1;
    }
}

void crossover::NBX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::mt19937 *rng)
{
    child = parent2;
    std::unordered_set<uint> nodes;
    std::uniform_int_distribution<uint> nodeRng(0, node_cnt - 1);

    uint cnt = nodeRng(*rng) + 1;
    while (nodes.size() < cnt) 
        nodes.insert(nodeRng(*rng));

    // removing chosen nodes from child
    for (auto node : nodes)
    {
        auto node_removed_end = std::remove(child.begin(), child.end(), node);
        child.erase(node_removed_end, child.end());
    }
    // inserting removed nodes on indexes as in parent1
    for (uint i = 0; i < parent1.size(); i++)
    {
        if (nodes.count(parent1[i]))
        {
            auto it = i < child.size() ? child.begin() + i : child.end();
            child.insert(it, parent1[i]);
        }
    }
}

void crossover::PBX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::mt19937 *rng)
{
    const uint invalid = node_cnt;
    std::uniform_int_distribution<uint> positionRng(0, parent1.size() - 1);
    uint positionCnt = positionRng(*rng) + 1;
    vector<uint> nodeFreq(node_cnt, 0);
    std::set<uint> insertIdx = {};


    while (insertIdx.size() < positionCnt)
    {
        uint idx = positionRng(*rng);
        uint node = parent1[idx];
        if (insertIdx.insert(idx).second)
            nodeFreq[node] += 1;
    }

    child = parent2;
    vector<uint> randIdxs(child.size());
    std::iota(randIdxs.begin(), randIdxs.end(), 0);
    std::shuffle(randIdxs.begin(), randIdxs.end(), *rng);
    vector<uint> remove(0);
    // randomly iter over child and erase at most the same number of nodes as chosen to insert
    for (auto idx : randIdxs)
    {
        uint node = child[idx];
        if (nodeFreq[node] > 0)
        {
            nodeFreq[node] -= 1;
            remove.push_back(idx);
        }
    }

    std::sort(remove.begin(), remove.end(), std::greater<>());
    for (auto idx : remove)
        child.erase(child.begin() + idx);   

    for (auto idx : insertIdx)
        child.insert(child.begin() + std::min(idx, (uint)child.size()), parent1[idx]);
    
}

void crossover::OX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::mt19937 *rng)
{
    std::uniform_int_distribution<uint> randPosition(0, parent1.size());
    std::uniform_real_distribution<> randNorm(0, 1);
    uint idx1, idx2;

    do
    {
        idx1 = randPosition(*rng);
        idx2 = randPosition(*rng);
    } while (idx1 >= idx2);

    vector<uint> core(parent1.begin() + idx1, parent1.begin() + idx2);
    vector<uint> core_frequency(node_cnt, 0);

    for (auto node : core)
        core_frequency[node]++;

    child = parent2;
    vector<uint> randIdxs(parent2.size());
    std::iota(randIdxs.begin(), randIdxs.end(), 0);
    std::shuffle(randIdxs.begin(), randIdxs.end(), *rng);

    vector<uint> remove(0);
    for (auto idx : randIdxs)
    {
        uint node = child[idx];
        if (core_frequency[node] > 0)
        {
            remove.push_back(idx);
            core_frequency[node]--;
        }
    }
    std::sort(remove.begin(), remove.end(), std::greater<>());
    for (auto idx : remove)
        child.erase(child.begin() + idx);

    uint start = std::min(idx1, (uint)child.size());
    child.insert(child.begin() + start, core.begin(), core.end());
}

void crossover::OBX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> freq1, vector<uint> freq2, std::mt19937 *rng)
{
    uint nodeCnt = freq1.size();
    std::uniform_int_distribution<uint> randNodeRng(0, nodeCnt - 1);
    uint randNodeCnt = randNodeRng(*rng) + 1;
    std::unordered_set<uint> nodes = {};

    int freqBalance = 0;
    while (nodes.size() != randNodeCnt || freqBalance != 0)
    {
        if (nodes.size() == randNodeCnt)
        {
            nodes = {};
            freqBalance = 0;
            randNodeCnt = randNodeRng(*rng) + 1;
        }
        uint node = randNodeRng(*rng);

        if (nodes.insert(node).second)                             // if insertion took place
            freqBalance = freqBalance + freq1[node] - freq2[node]; // sum of node frequencies must be same in both parents
    }

    std::queue<uint> toInsert;
    for (uint node : parent1)
        if (nodes.count(node))
            toInsert.push(node);

    child = parent2;
    for (uint &node : child)
        if (nodes.count(node))
        {
            node = toInsert.front();
            toInsert.pop();
        }
}

void crossover::CX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint nodeCnt, std::mt19937 *rng)
{
    const uint invalid = nodeCnt;
    while (parent1.size() < parent2.size())
    {
        parent1.push_back(invalid);
    }
    while (parent2.size() < parent1.size())
    {
        parent2.push_back(invalid);
    }

    std::unordered_map<uint, vector<uint>> nodeMap;
    for (uint i = 0; i <= nodeCnt; i++)
        nodeMap[i] = {};
    for (uint i = 0; i < parent1.size(); i++)
        nodeMap.at(parent1[i]).push_back(i);

    child = vector<uint>(parent1.size(), invalid);
    uint node = parent1[0];
    queue<uint> toInsert;
    toInsert.push(node);

    while (toInsert.size() > 0)
    {
        node = toInsert.front();
        toInsert.pop();
        for (auto i : nodeMap.at(node))
        {
            child[i] = node;
            toInsert.push(parent2[i]);
        }
        nodeMap.at(node) = {};
    }

    for (uint i = 0; i < child.size(); i++)
        if (child[i] == invalid)
            child[i] = parent2[i];

    auto end = std::remove(child.begin(), child.end(), invalid);
    child.erase(end, child.end());
}

void crossover::HGreX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::function<permutator::fitness_t(vector<uint>)> getFitness, std::mt19937 *rng)
{
    std::function<uint(vector<uint>, vector<uint>)> f = [getFitness](vector<uint> permutation, vector<uint> feasibleNext) -> uint
    {
        fitness_t bestFitness = UINT_MAX;
        uint bestNode;

        for (auto node : feasibleNext)
        {
            permutation.push_back(node);
            fitness_t fitness = getFitness(permutation);
            if (fitness < bestFitness)
            {
                bestFitness = fitness;
                bestNode = node;
            }
            permutation.erase(permutation.end() - 1);
        }
        return bestNode;
    };

    crossover::HXHelper(parent1, parent2, child, lbs, ubs, f, rng);
}

void crossover::HRndX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::function<permutator::fitness_t(vector<uint>)> getFitness, std::mt19937 *rng)
{
    std::function<uint(vector<uint>, vector<uint>)> f = [rng](vector<uint> permutation, vector<uint> feasibleNext) -> uint
    {
        std::uniform_int_distribution<uint> randNode(0, feasibleNext.size() - 1);
        return feasibleNext[randNode(*rng)];
    };

    crossover::HXHelper(parent1, parent2, child, lbs, ubs, f, rng);
}

void crossover::HProX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::function<permutator::fitness_t(vector<uint>)> getFitness, std::mt19937 *rng)
{
    std::function<uint(vector<uint>, vector<uint>)> f = [getFitness, rng](vector<uint> permutation, vector<uint> feasibleNext) -> uint
    {
        vector<double> probabilities = {};
        double min = INFINITY;

        for (auto node : feasibleNext)
        {
            permutation.push_back(node);
            double fitness = getFitness(permutation);
            probabilities.push_back(fitness);
            if (fitness < min)
                min = fitness;
            permutation.erase(permutation.end() - 1);
        }

        double sum = 0;
        for (auto &p : probabilities)
        {
            p = min / p;
            sum += p;
        }

        std::uniform_real_distribution<> randNum(0, sum);
        double rand = randNum(*rng);
        uint i = 0;
        for (; i < probabilities.size(); i++)
        {
            rand -= probabilities[i];
            if (rand < 0)
                break;
        }
        return feasibleNext[i];
    };

    crossover::HXHelper(parent1, parent2, child, lbs, ubs, f, rng);
}

void crossover::HXHelper(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::function<uint(vector<uint>, vector<uint>)> getNextNode, std::mt19937 *rng)
{
    child.clear();
    uint node_cnt = lbs.size();
    vector<uint> freq(node_cnt, 0);

    std::unordered_map<uint, vector<uint>> nextMap;
    for (uint i = 0; i < node_cnt; i++)
        nextMap[i] = {};
    for (uint i = 0; i < parent1.size(); i++)
        nextMap.at(parent1[i]).push_back(parent1[(i + 1) % parent1.size()]);
    for (uint i = 0; i < parent2.size(); i++)
        nextMap.at(parent2[i]).push_back(parent2[(i + 1) % parent2.size()]);

    std::uniform_int_distribution<uint> randPosition(0, parent1.size() - 1);
    child.push_back(parent1[randPosition(*rng)]);
    freq[child.back()] += 1;

    while (true)
    {
        uint current = child.back();
        vector<uint> feasibleNext(0);

        // check if any neighbor can be inserted
        for (auto node : nextMap.at(current))
            if (freq[node] < ubs[node])
                feasibleNext.push_back(node);
        // if not insert node bellow lbs
        if (feasibleNext.empty())
            for (uint node = 0; node < node_cnt; node++)
                if (freq[node] < lbs[node])
                    feasibleNext.push_back(node);
        // no node bellow lbs -> can break
        if (feasibleNext.size() == 0)
            break;

        uint next = getNextNode(child, feasibleNext);
        child.push_back(next);
        freq[next] += 1;

        // erase edge from current
        auto it = find(nextMap.at(current).begin(), nextMap.at(current).end(), next);
        if (it != nextMap.at(current).end())
            nextMap.at(current).erase(it);
    }
}

// void crossover::PMX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> freq1, vector<uint> freq2, std::mt19937 *rng)
// {
//     if (freq1 != freq2)
//     {
//         child = parent1;
//         return;
//     }
//     std::uniform_int_distribution<uint> randPosition(0, parent1.size());
//     uint start, end;
//     do
//     {
//         start = randPosition(*rng);
//         end = randPosition(*rng);
//     } while (start >= end);

//     LOG(start);
//     LOG(end);

//     vector<uint> core1(parent1.begin() + start, parent1.begin() + end);
//     vector<uint> core2(parent2.begin() + start, parent2.begin() + end);

//     child = parent2;
//     vector<uint> randIdxs(child.size());
//     std::iota(randIdxs.begin(), randIdxs.end(), 0);
//     randIdxs.erase(randIdxs.begin() + start, randIdxs.begin() + end);
//     std::shuffle(randIdxs.begin(), randIdxs.end(), *rng);

//     std::unordered_map<uint, vector<uint>> partialMap;
//     for (uint i = 0; i < freq1.size(); i++)
//         partialMap[i] = {};
//     for (uint i = 0; i < core1.size(); i++)
//     {
//         partialMap.at(core1[i]).push_back(core2[i]);
//         child[start + i] = core1[i];
//     }

//     std::map<uint, vector<uint>> inserts;
//     for (auto idx : randIdxs)
//     {
//         auto node = child[idx];
//         if (partialMap.at(node).size() == 0)
//             continue;

//         inserts[idx] = partialMap.at(node);
//         partialMap.at(node) = {};
//         uint i = 0;

//         while (i < inserts.at(idx).size())
//         {
//             node = inserts.at(idx)[i];
//             inserts.at(idx).insert(inserts.at(idx).end(), partialMap.at(node).begin(), partialMap.at(node).end());
//             if (partialMap.at(node).size() > 0)
//                 inserts.at(idx).erase(inserts.at(idx).begin() + i);
//             else
//                 i++;
//             partialMap.at(node) = {};
//         }
//     }
//     LOG("--");
//     for (auto [index, values] : inserts)
//     {
//         LOG(index);
//         child.erase(child.begin() + index);
//         child.insert(child.begin() + index, values.begin(), values.end());
//     }
//     LOG("--");
// }