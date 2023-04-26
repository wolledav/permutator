
#include "operator.hpp"

using permutator::fitness_t;
using std::queue;
using std::vector;


//**********************************************************************************************************************
// BASIC OPERATORS
//**********************************************************************************************************************


// inserts 'node' at 'position'
void oprtr::insert(vector<uint> &permutation, vector<uint> &frequency, vector<uint> ubs, uint node, uint position)
{
    if (frequency[node] < ubs[node] && position <= permutation.size())
    {
        permutation.insert(permutation.begin() + position, node);
        frequency[node]++;
    }
}

// appends 'node' at the end of 'permutation'
void oprtr::append(vector<uint> &permutation, vector<uint> &frequency, vector<uint> ubs, uint node)
{
    if (frequency[node] < ubs[node])
    {
        permutation.emplace_back(node);
        frequency[node]++;
    }
}

// removes 'node' at 'position'
void oprtr::remove(vector<uint> &permutation, vector<uint> &frequency, vector<uint> lbs, uint position)
{
    if (position < permutation.size() && frequency[permutation[position]] > lbs[permutation[position]])
    {
        frequency[permutation[position]]--;
        permutation.erase(permutation.begin() + position);
    }
}

// moves subsequence starting at 'from' of length 'length' to 'to', reverses subsequence if reverse==true
// relocate([1,2,3,4], 1, 2, 2, false) -> [1,4,2,3]
void oprtr::relocate(vector<uint> &permutation, uint from, uint to, uint length, bool reverse)
{
    // LOG(from); LOG(to); LOG(length); LOG(permutation.size());
    if (from + length > permutation.size() || to > (int)permutation.size() - length)
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

    bool clean = false;
    if (positions == nullptr)
    {
        positions = new vector<uint>(0);
        clean = true;
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

    if (clean)
        delete positions;
}

// swaps all occurrences of 'nodeX' and 'nodeY', if frequencies are in bounds
// exchangeIds([1,2,3,1,2,1], freq, lbs, ubs, 1, 2) -> [2,1,3,2,1,2]
void oprtr::exchangeIds(vector<uint> &permutation, vector<uint> &frequency, vector<uint> lbs, vector<uint> ubs, uint nodeX, uint nodeY)
{
    if (frequency[nodeX] < lbs[nodeY] || frequency[nodeY] < lbs[nodeX] ||
        frequency[nodeX] > ubs[nodeY] || frequency[nodeY] > ubs[nodeX])
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
void oprtr::exchangeNIds(vector<uint> &permutation, vector<uint> &frequency, vector<uint> lbs, vector<uint> ubs, uint nodeX, uint nodeY, uint maxSwap)
{
    uint counter1 = 0, counter2 = 0;
    vector<uint> new_perm = permutation;
    for (unsigned int &node : new_perm)
    { // for all nodes in new_perm
        if (node == nodeX && counter1 < maxSwap && frequency[nodeX] > lbs[nodeX] && frequency[nodeY] < ubs[nodeY])
        { // if i swapped less than n times
            counter1++;
            node = nodeY;
            frequency[nodeX]--;
            frequency[nodeY]++;
        }
        else if (node == nodeY && counter2 < maxSwap&& frequency[nodeY] > lbs[nodeY] && frequency[nodeX] < ubs[nodeX])
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



//**********************************************************************************************************************
// CONSTRUCTIONS
//**********************************************************************************************************************



// Adds nodes to random positions of the solution, until all nodes are at their LBs.
// Uses permutation as initial solution
void construct::random(vector<uint> &permutation, vector<uint> &frequency, vector<uint> lbs, std::mt19937 *rng)
{

    uint rnd_idx;
    uint node_cnt = frequency.size();

    for (uint node_id = 0; node_id < node_cnt; node_id++)
    {
        while (frequency[node_id] < lbs[node_id])
        {
            std::uniform_int_distribution<uint> uni(0, permutation.size());
            rnd_idx = uni(*rng);
            permutation.insert(permutation.begin() + rnd_idx, node_id);
            frequency[node_id]++;
        }
    }
}

// Creates a random permutation of nodes.
// Replicates the permutation, until the solution is not valid w.r.t. LBs and UBs.
// Uses permutation as initial solution
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

//**********************************************************************************************************************
// CROSSOVER
//**********************************************************************************************************************

void crossover::ERX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::mt19937 *rng)
{
    child.clear();
    uint node_cnt = lbs.size();
    vector<uint> frequency(node_cnt, 0);

    std::unordered_map<uint, vector<uint>> neighborhood_parent1;
    std::unordered_map<uint, vector<uint>> neighborhood_parent2;
    for (uint node = 0; node < node_cnt; node++)
    {
        neighborhood_parent1[node] = {};
        neighborhood_parent2[node] = {};
    }

    for (uint i = 0; i < parent1.size(); i++)
    {
        uint node = parent1[i];
        uint next = parent1[i + 1 == parent1.size() ? 0 : i + 1]; // wrap around permutation
        uint prev = parent1[i == 0 ? parent1.size() - 1 : i - 1]; // wrap around permutation
        neighborhood_parent1.at(node).push_back(next);
        neighborhood_parent1.at(node).push_back(prev);
    }

    for (uint i = 0; i < parent2.size(); i++)
    {
        uint node = parent2[i];
        uint next = parent2[i + 1 == parent2.size() ? 0 : i + 1]; // wrap around permutation
        uint prev = parent2[i == 0 ? parent2.size() - 1 : i - 1]; // wrap around permutation
        neighborhood_parent2.at(node).push_back(next);
        neighborhood_parent2.at(node).push_back(prev);
    }

    std::unordered_set<uint> joined_neighborhood;
    std::unordered_map<uint, vector<uint>> neighborhood;

    for (uint node = 0; node < node_cnt; node++)
    {
        joined_neighborhood.insert(neighborhood_parent1.at(node).begin(), neighborhood_parent1.at(node).end());
        joined_neighborhood.insert(neighborhood_parent2.at(node).begin(), neighborhood_parent2.at(node).end());
        neighborhood[node] = {};
        for (auto n : joined_neighborhood)
        {
            neighborhood.at(node).insert(
                neighborhood.at(node).begin(),
                std::max(
                    std::count(neighborhood_parent1.at(node).begin(), neighborhood_parent1.at(node).end(), n),
                    std::count(neighborhood_parent2.at(node).begin(), neighborhood_parent2.at(node).end(), n)),
                n);
        }
        joined_neighborhood.clear();
    }

    child.push_back(parent1[0]);
    frequency[parent1[0]] += 1;

    while (true)
    {
        uint current = child.back();
        vector<uint> possibleNext(0);

        // check if any neighbor can be inserted...
        for (auto node : neighborhood.at(current))
            if (frequency[node] < ubs[node])
            {
                // choose a neighbor with fewest neighbors
                uint neighborhood_size = possibleNext.size() == 0 ? UINT_MAX : neighborhood.at(possibleNext[0]).size();
                if (neighborhood.at(node).size() < neighborhood_size)
                    possibleNext = {node};
                if (neighborhood.at(node).size() == neighborhood_size)
                    possibleNext.push_back(node);
            }
        // ...if not insert node bellow lbs
        if (possibleNext.empty())
            for (uint node = 0; node < node_cnt; node++)
                if (frequency[node] < lbs[node])
                    possibleNext.push_back(node);
        // no node bellow lbs -> end
        if (possibleNext.size() == 0)
            break;

        std::uniform_int_distribution<uint> randNode(0, possibleNext.size() - 1);
        uint next = possibleNext[randNode(*rng)];
        child.push_back(next);
        frequency[next] += 1;

        // erase edge from current and next
        auto it = find(neighborhood.at(current).begin(), neighborhood.at(current).end(), next);
        if (it != neighborhood.at(current).end())
        {
            neighborhood.at(current).erase(it);
            it = find(neighborhood.at(next).begin(), neighborhood.at(next).end(), current);
            neighborhood.at(next).erase(it);
        }
    }
}

void crossover::AEX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::mt19937 *rng)
{
    child.clear();
    uint node_cnt = lbs.size();
    vector<uint> frequency(node_cnt, 0);

    std::unordered_map<uint, vector<uint>> parent1_next_node;
    std::unordered_map<uint, vector<uint>> parent2_next_node;
    for (uint node = 0; node < node_cnt; node++)
    {
        parent1_next_node[node] = {};
        parent2_next_node[node] = {};
    }

    for (uint i = 0; i < parent1.size() - 1; i++)
        parent1_next_node.at(parent1[i]).push_back(parent1[i + 1]);
    for (uint i = 0; i < parent2.size() - 1; i++)
        parent2_next_node.at(parent2[i]).push_back(parent2[i + 1]);

    uint temp = 0;
    std::unordered_map<uint, vector<uint>> *next_node;
    child.push_back(parent1[0]);
    frequency[parent1[0]] += 1;

    while (true)
    {
        next_node = temp++ % 2 == 0 ? &parent1_next_node : &parent2_next_node;
        uint current = child.back();
        vector<uint> possible_next_node(0);

        // check if any neighbor can be inserted...
        for (auto node : next_node->at(current))
            // first feasible node is selected
            if (frequency[node] < ubs[node])
            {
                possible_next_node.push_back(node);
                break;
            }
        // ...if not insert node bellow lbs
        if (possible_next_node.empty())
            for (uint node = 0; node < node_cnt; node++)
                if (frequency[node] < lbs[node])
                    possible_next_node.push_back(node);

        uint next;
        // no node bellow lbs -> end
        if (possible_next_node.size() == 0)
        {
            break;
        }
        // first feasible node is selected
        else if (possible_next_node.size() == 1)
        {
            next = possible_next_node[0];
            // erase edge from current
            auto it = find(next_node->at(current).begin(), next_node->at(current).end(), next);
            if (it != next_node->at(current).end())
                next_node->at(current).erase(it);
        }
        // no possible alternating node, chooses random from possible nodes
        else
        {
            std::uniform_int_distribution<uint> randNode(0, possible_next_node.size() - 1);
            next = possible_next_node[randNode(*rng)];
        }
        child.push_back(next);
        frequency[next] += 1;
    }
}

void crossover::NBX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(vector<uint> &x, vector<uint> &y, uint gap_node)> alignmentFunction)
{
    const uint gap_node = node_cnt;
    alignmentFunction(parent1, parent2, gap_node);

    child = parent2;
    std::unordered_set<uint> nodes;
    std::uniform_int_distribution<uint> node_rng(0, node_cnt - 1);

    uint cnt = node_rng(*rng) + 1;
    while (nodes.size() < cnt)
        nodes.insert(node_rng(*rng));

    // removing chosen nodes from child
    for (auto node : nodes)
    {
        auto new_end = std::remove(child.begin(), child.end(), node);
        child.erase(new_end, child.end());
    }
    // inserting removed nodes on indexes as in parent1
    for (uint i = 0; i < parent1.size(); i++)
        if (nodes.count(parent1[i]))
            child.insert(child.begin() + std::min((uint)child.size(), i), parent1[i]);

    alignment::removeGap(child, gap_node);
}

void crossover::PBX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(vector<uint> &x, vector<uint> &y, uint gap_node)> alignmentFunction)
{
    const uint gap_node = node_cnt;
    alignmentFunction(parent1, parent2, gap_node);
    // LOG(1);
    std::uniform_int_distribution<uint> position_rng(0, parent1.size() - 1);
    uint insert_cnt = std::max((int)position_rng(*rng) - (int)std::count(parent1.begin(), parent1.end(), gap_node), 0);
    vector<uint> node_frequencies(node_cnt + 1, 0);
    std::set<uint> insert_idxs = {};
    // LOG(1.5);
    while (insert_idxs.size() < insert_cnt)
    {
        uint idx = position_rng(*rng);
        uint node = parent1[idx];
        if (node != gap_node && insert_idxs.insert(idx).second)
            node_frequencies[node] += 1;
    }
    // LOG(2);
    child = parent2;
    vector<uint> rand_idxs(child.size());
    std::iota(rand_idxs.begin(), rand_idxs.end(), 0);
    std::shuffle(rand_idxs.begin(), rand_idxs.end(), *rng);
    vector<uint> remove(0);
    // LOG(3);
    // randomly iter over child and erase at most the same number of nodes as chosen to insert
    for (auto idx : rand_idxs)
    {
        uint node = child[idx];
        if (node_frequencies[node] > 0)
        {
            node_frequencies[node] -= 1;
            remove.push_back(idx);
        }
    }
    // LOG(4);
    std::sort(remove.begin(), remove.end(), std::greater<>());
    for (auto idx : remove)
        child.erase(child.begin() + idx);
    // LOG(5);
    for (auto idx : insert_idxs)
        child.insert(child.begin() + idx, parent1[idx]);
    // LOG(6);
    alignment::removeGap(child, gap_node);
    // LOG(7);
}

void crossover::OX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(vector<uint> &x, vector<uint> &y, uint gap_node)> alignmentFunction)
{
    const uint gap_node = node_cnt;
    alignmentFunction(parent1, parent2, gap_node);

    std::uniform_int_distribution<uint> position_rng(0, parent1.size());
    uint idx1, idx2;
    do
    {
        idx1 = position_rng(*rng);
        idx2 = position_rng(*rng);
    } while (idx1 >= idx2);

    vector<uint> core(parent1.begin() + idx1, parent1.begin() + idx2);
    vector<uint> core_frequency(node_cnt + 1, 0);

    for (auto node : core)
        core_frequency[node]++;

    child = parent2;

    vector<uint> tail = vector<uint>(child.begin() + idx2, child.end());
    child.erase(child.begin() + idx2, child.end());
    child.insert(child.begin(), tail.begin(), tail.end());

    vector<uint> rand_idxs(child.size());
    std::iota(rand_idxs.begin(), rand_idxs.end(), 0);
    std::shuffle(rand_idxs.begin(), rand_idxs.end(), *rng);

    vector<uint> remove(0);
    for (auto idx : rand_idxs)
    {
        uint node = child[idx];
        if (node != gap_node && core_frequency[node] > 0)
        {
            remove.push_back(idx);
            core_frequency[node]--;
        }
    }

    std::sort(remove.begin(), remove.end(), std::greater<>());
    for (auto idx : remove)
        child.erase(child.begin() + idx);

    uint i = 0;
    while (i < tail.size()) // && i < child.size()
    {
        core.push_back(child[i++]);
    }

    core.insert(core.begin(), child.begin() + i, child.end());
    child = core;

    alignment::removeGap(child, gap_node);
}

void crossover::OBX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> freq1, vector<uint> freq2, std::mt19937 *rng)
{
    uint node_cnt = freq1.size();
    std::uniform_int_distribution<uint> node_rng(0, node_cnt - 1);
    uint rand_node_cnt = node_rng(*rng) + 1;
    std::unordered_set<uint> nodes = {};

    int frequency_balance = 0;
    while (nodes.size() != rand_node_cnt || frequency_balance != 0)
    {
        if (nodes.size() == rand_node_cnt)
        {
            nodes = {};
            frequency_balance = 0;
            rand_node_cnt = node_rng(*rng) + 1;
        }
        uint node = node_rng(*rng);

        if (nodes.insert(node).second)                                         // if insertion took place
            frequency_balance = frequency_balance + freq1[node] - freq2[node]; // sum of node frequencies must be same in both parents
    }

    queue<uint> to_insert;
    for (uint node : parent1)
        if (nodes.count(node))
            to_insert.push(node);

    child = parent2;
    for (uint &node : child)
        if (nodes.count(node))
        {
            node = to_insert.front();
            to_insert.pop();
        }
}

void crossover::CX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(vector<uint> &x, vector<uint> &y, uint gap_node)> alignmentFunction)
{
    const uint gap_node = node_cnt;
    alignmentFunction(parent1, parent2, gap_node);

    // for (auto n : parent1)
    //     std::cout << n << " ";
    // LOG("\n");

    // for (auto n : parent2)
    //     std::cout << n << " ";
    // LOG("\n\n");

    std::unordered_map<uint, vector<uint>> node_idxs;
    for (uint i = 0; i <= node_cnt; i++)
        node_idxs[i] = {};
    for (uint i = 0; i < parent1.size(); i++)
        node_idxs.at(parent1[i]).push_back(i);
    node_idxs.at(gap_node) = {};

    child = vector<uint>(parent1.size(), gap_node);
    std::stack<uint> stack;
    stack.push(0);

    uint invalid_idx = child.size();
    vector<uint> walk(child.size(), invalid_idx);

    bool search = true;
    while (stack.size() > 0 && search)
    {
        uint idx = stack.top();
        stack.pop();
        // LOG(idx);
        // LOG(parent2[idx]);
        // LOG("-");

        for (auto i : node_idxs.at(parent2[idx]))
        {
            // LOG(i);
            if (walk[i] == invalid_idx)
            {
                stack.push(i);
                walk[i] = idx;
                if (i == 0)
                {
                    search = false;
                    break;
                }
            }
        }
        // LOG("---");
    }

    if (walk[0] != invalid_idx)
    {
        uint idx = walk[0];
        while (idx != 0)
        {
            child[idx] = parent1[idx];
            idx = walk[idx];
        }
        child[0] = parent1[0];
    }

    // for (auto n : child)
    //     std::cout << n << " ";
    // LOG(" ");

    for (uint idx = 0; idx < child.size(); idx++)
        if (child[idx] == gap_node)
            child[idx] = parent2[idx];

    alignment::removeGap(child, gap_node);
}

void crossover::HGreX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::function<permutator::fitness_t(vector<uint>)> getFitness, std::mt19937 *rng)
{
    std::function<uint(vector<uint>, vector<uint>)> f = [getFitness](vector<uint> permutation, vector<uint> next_feasible_nodes) -> uint
    {
        fitness_t best_fitness = MAX_FITNESS;
        uint best_node;

        for (auto node : next_feasible_nodes)
        {
            permutation.push_back(node);
            fitness_t fitness = getFitness(permutation);
            if (fitness < best_fitness)
            {
                best_fitness = fitness;
                best_node = node;
            }
            permutation.erase(permutation.end() - 1);
        }
        return best_node;
    };

    crossover::HXHelper(parent1, parent2, child, lbs, ubs, f, rng);
}

void crossover::HRndX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::mt19937 *rng)
{
    std::function<uint(vector<uint>, vector<uint>)> f = [rng](vector<uint> permutation, vector<uint> next_feasible_nodes) -> uint
    {
        std::uniform_int_distribution<uint> node_rng(0, next_feasible_nodes.size() - 1);
        return next_feasible_nodes[node_rng(*rng)];
    };

    crossover::HXHelper(parent1, parent2, child, lbs, ubs, f, rng);
}

void crossover::HProX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::function<permutator::fitness_t(vector<uint>)> getFitness, std::mt19937 *rng)
{
    std::function<uint(vector<uint>, vector<uint>)> f = [getFitness, rng](vector<uint> permutation, vector<uint> next_feasible_nodes) -> uint
    {
        vector<double> probabilities = {};
        double min = INFINITY;

        for (auto node : next_feasible_nodes)
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
        return next_feasible_nodes[i];
    };

    crossover::HXHelper(parent1, parent2, child, lbs, ubs, f, rng);
}

void crossover::HXHelper(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, std::function<uint(vector<uint>, vector<uint>)> getNextNode, std::mt19937 *rng)
{
    child.clear();
    uint node_cnt = lbs.size();
    vector<uint> frequency(node_cnt, 0);

    std::unordered_map<uint, vector<uint>> next_node;
    for (uint i = 0; i < node_cnt; i++)
        next_node[i] = {};
    for (uint i = 0; i < parent1.size(); i++)
        next_node.at(parent1[i]).push_back(parent1[(i + 1) % parent1.size()]);
    for (uint i = 0; i < parent2.size(); i++)
        next_node.at(parent2[i]).push_back(parent2[(i + 1) % parent2.size()]);

    std::uniform_int_distribution<uint> position_rng(0, parent1.size() - 1);
    child.push_back(parent1[position_rng(*rng)]);
    frequency[child.back()] += 1;

    while (true)
    {
        uint current = child.back();
        vector<uint> next_feasible_nodes(0);

        // check if any neighbor can be inserted...
        for (auto node : next_node.at(current))
            if (frequency[node] < ubs[node])
                next_feasible_nodes.push_back(node);

        // ...if not insert node bellow lbs
        if (next_feasible_nodes.empty())
            for (uint node = 0; node < node_cnt; node++)
                if (frequency[node] < lbs[node])
                    next_feasible_nodes.push_back(node);

        // no node bellow lbs -> can break
        if (next_feasible_nodes.size() == 0)
            break;

        uint next = getNextNode(child, next_feasible_nodes);
        child.push_back(next);
        frequency[next] += 1;

        // erase edge from current
        auto it = find(next_node.at(current).begin(), next_node.at(current).end(), next);
        if (it != next_node.at(current).end())
            next_node.at(current).erase(it);
        
    }
}

// from QAP Xover
void crossover::rULX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> lbs, vector<uint> ubs, bool random, std::mt19937 *rng, std::function<void(vector<uint> &x, vector<uint> &y, uint gap_node)> alignmentFunction)
{

    const uint node_cnt = lbs.size();
    const uint gap_node = node_cnt;

    alignmentFunction(parent1, parent2, gap_node);
    lbs.push_back(0);
    ubs.push_back(0);

    child = vector<uint>(parent1.size(), gap_node);
    vector<uint> frequency(node_cnt + 1, 0);

    std::uniform_int_distribution<uint> parent_rng(0, 1);

    vector<uint> idxs(child.size());
    std::iota(idxs.begin(), idxs.end(), 0);
    if (random)
        std::shuffle(idxs.begin(), idxs.end(), *rng);

    for (auto idx : idxs)
    {
        if (parent1[idx] == parent2[idx])
        {
            child[idx] = parent1[idx];
            frequency[parent1[idx]]++;
        }
    }

    for (auto xbs : {lbs, ubs})
    {
        for (auto idx : idxs)
        {
            if (child[idx] != gap_node)
                continue;

            uint node1 = parent1[idx];
            uint node2 = parent2[idx];

            if (frequency[node1] < xbs[node1] && frequency[node2] < xbs[node2])
            {
                uint node = parent_rng(*rng) ? node1 : node2;
                child[idx] = node;
                frequency[node]++;
            }
            else if (frequency[node1] < xbs[node1])
            {
                child[idx] = node1;
                frequency[node1]++;
            }
            else if (frequency[node2] < xbs[node2])
            {
                child[idx] = node2;
                frequency[node2]++;
            }
        }
    }

    vector<uint> bellow_lbs = {};
    for (uint node = 0; node < node_cnt; node++)
        for (int i = frequency[node] - lbs[node]; i < 0; i++)
            bellow_lbs.push_back(node);

    std::shuffle(bellow_lbs.begin(), bellow_lbs.end(), *rng);

    for (auto idx : idxs)
    {
        if (child[idx] == gap_node && bellow_lbs.size() > 0)
        {
            child[idx] = bellow_lbs.back();
            bellow_lbs.pop_back();
        }
    }

    for (auto node : bellow_lbs)
        child.push_back(node);

    alignment::removeGap(child, gap_node);
}

// from QAP Xover
void crossover::EULX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, std::function<fitness_t(vector<uint>)> getFitness)
{
    vector<uint> candidate_list = {};

    for (uint idx = 0; idx < child.size(); idx++)
    {
        bool in_parent = false;
        if (idx < parent1.size())
            in_parent = in_parent || parent1[idx] == child[idx];
        if (idx < parent2.size())
            in_parent = in_parent || parent2[idx] == child[idx];

        if (!in_parent)
            candidate_list.push_back(idx);
    }
    uint invalid = candidate_list.size();
    uint idx1 = 0;
    uint idx2 = 0;
    while (idx1 != invalid && idx2 != invalid)
    {
        idx1 = invalid;
        idx2 =  invalid;
        fitness_t min_fitness = getFitness(child);
        vector<uint> best_permutation = child;
        for (int i = 0; i < (int)candidate_list.size() - 1; i++)
        {
            for (int j = i + 1; j < candidate_list.size(); j++)
            {
                std::swap(child[candidate_list[i]], child[candidate_list[j]]);
                fitness_t fitness = getFitness(child);
                if (fitness < min_fitness)
                {
                    idx1 = i;
                    idx2 = j;
                    min_fitness = fitness;
                    best_permutation = child;
                }
                std::swap(child[candidate_list[i]], child[candidate_list[j]]);
            }
        }
        if (idx1 != invalid && idx2 != invalid){
            candidate_list.erase(candidate_list.begin() + idx2);
            candidate_list.erase(candidate_list.begin() + idx1);
        }
        child = best_permutation;
    }
}

void crossover::UPMX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::mt19937 *rng, std::function<void(vector<uint> &x, vector<uint> &y, uint gap_node)> alignmentFunction)
{
    const uint gap_node = node_cnt;
    alignmentFunction(parent1, parent2, gap_node);

    std::unordered_map<uint, vector<uint>> node_idxs;
    for (uint i = 0; i <= node_cnt; i++)
        node_idxs[i] = {};
    for (uint i = 0; i < parent2.size(); i++)
        node_idxs.at(parent2[i]).push_back(i);
    node_idxs.at(gap_node) = {};

    child = parent1;
    std::uniform_int_distribution<uint> idx1_rng(0, child.size() - 1);

    for (uint i = 0; i < child.size()/3; i++)
    {
        uint idx1 = idx1_rng(*rng);
        uint node = child[idx1];
        if (node_idxs.at(node).size() > 0)
        {
            std::uniform_int_distribution<uint> idx2_rng(0, node_idxs.at(node).size() - 1);
            uint idx2 = node_idxs.at(node)[idx2_rng(*rng)];
            std::swap(child[idx1], child[idx2]);
        }
    }

    alignment::removeGap(child, gap_node);
}

void crossover::SPX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::function<fitness_t(vector<uint>, uint)> getFitness, std::mt19937 *rng, std::function<void(vector<uint> &x, vector<uint> &y, uint gap_node)> alignmentFunction)
{
    const uint gap_node = node_cnt;
    alignmentFunction(parent1, parent2, gap_node);

    child = parent1;
    fitness_t best_fitness = getFitness(child, gap_node);

    std::uniform_int_distribution<uint> position_rng(0, child.size() - 1);
    uint end = position_rng(*rng);
    int idx = end;

    vector<uint> idxs(child.size());
    std::iota(idxs.begin(), idxs.end(), 0);
    std::shuffle(idxs.begin(), idxs.end(), *rng);

    do
    {
        idx = (idx + 1) % child.size();
        vector<uint> child1 = parent1;
        vector<uint> child2 = parent2;

        if (child1[idx] == child2[idx] || child1[idx] == gap_node || child2[idx] == gap_node)
            continue;

        for (auto i : idxs)
        {
            if (child1[i] == parent2[idx])
            {
                child1[i] = parent1[idx];
                child1[idx] = parent2[idx];
                break;
            }
        }

        for (auto i : idxs)
        {
            if (child2[i] == parent1[idx])
            {
                child2[i] = parent2[idx];
                child2[idx] = parent1[idx];
                break;
            }
        }

        fitness_t child1_fitness = getFitness(child1, gap_node);
        fitness_t child2_fitness = getFitness(child2, gap_node);

        if (child1_fitness <= child2_fitness)
        {
            parent1 = child1;
            if (child1_fitness < best_fitness)
            {
                child = child1;
                best_fitness = child1_fitness;
            }
        }
        else
        {
            parent2 = child2;
            if (child2_fitness < best_fitness)
            {
                child = child2;
                best_fitness = child2_fitness;
            }
        }

    } while (idx != end);

    alignment::removeGap(child, gap_node);
}

void crossover::MPX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, uint node_cnt, std::mt19937 *rng)
{
    std::uniform_int_distribution<uint> position_rng(0, parent1.size());
    int min_size =  parent1.size()/2 > 10 ? 10 : 0;
    int idx1, idx2;
    do
    {
        idx1 = position_rng(*rng);
        idx2 = position_rng(*rng);
    } while (!(idx2 - idx1 >= min_size && idx2 - idx1 < parent1.size()/2));

    vector<uint> core(parent1.begin() + idx1, parent1.begin() + idx2);
    vector<uint> core_frequency(node_cnt + 1, 0);

    for (auto node : core)
        core_frequency[node]++;

    child = parent2;

    vector<uint> rand_idxs(child.size());
    std::iota(rand_idxs.begin(), rand_idxs.end(), 0);
    std::shuffle(rand_idxs.begin(), rand_idxs.end(), *rng);

    vector<uint> remove(0);
    for (auto idx : rand_idxs)
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

    child.insert(child.begin(), core.begin(), core.end());
}

void crossover::APX(vector<uint> parent1, vector<uint> parent2, vector<uint> &child, vector<uint> frequency)
{
    const uint gap_node = frequency.size();
    alignment::backfillOneGap(parent1, parent2, gap_node);
    frequency.push_back(0);
    child.clear();

    for (uint i = 0; i < parent1.size(); i++){
        if (frequency[parent1[i]] > 0){
            child.push_back(parent1[i]);
            frequency[parent1[i]]--;
        }
        else if (frequency[parent2[i]] > 0){
            child.push_back(parent2[i]);
            frequency[parent2[i]]--;
        }    
    }

    alignment::removeGap(child, gap_node);
}



//**********************************************************************************************************************
// ALIGNMENT
//**********************************************************************************************************************


void alignment::removeGap(std::vector<uint> &permutation, uint gap_node)
{
    auto new_end = std::remove(permutation.begin(), permutation.end(), gap_node);
    permutation.erase(new_end, permutation.end());
}

void  alignment::greedyUniform(vector<uint> &x, vector<uint> &y, uint gap_node, uint difference_penalty, uint gap_penalty)
{
    int i, j; // initialising variables
     
    int m = x.size(); // length of gene1
    int n = y.size(); // length of gene2
     
    // table for storing optimal substructure answers
    vector<vector<int>> dp;
    for (uint i = 0; i < y.size() + x.size() + 1; i++)
        dp.push_back(vector<int>(y.size() + x.size() + 1, 0));
    // int dp[n+m+1][n+m+1] = {0};
 
    // initialising the table
    for (i = 0; i <= (n+m); i++)
    {
        dp[i][0] = i * gap_penalty;
        dp[0][i] = i * gap_penalty;
    }   
 
    // calculating the minimum penalty
    for (i = 1; i <= m; i++)
    {
        for (j = 1; j <= n; j++)
        {
            if (x[i - 1] == y[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else
            {
                dp[i][j] = std::min({dp[i - 1][j - 1] + difference_penalty ,
                                dp[i - 1][j] + gap_penalty    ,
                                dp[i][j - 1] + gap_penalty    });
            }
        }
    }
 
    // Reconstructing the solution
    int l = n + m; // maximum possible length
     
    i = m; j = n;
     
    int xpos = l;
    int ypos = l;
 
    // Final answers for the respective strings
    vector<int> xans(l+1);
    vector<int> yans(l+1);
     
    while ( !(i == 0 || j == 0))
    {
        if (x[i - 1] == y[j - 1])
        {
            xans[xpos--] = (int)x[i - 1];
            yans[ypos--] = (int)y[j - 1];
            i--; j--;
        }
        else if (dp[i - 1][j - 1] + difference_penalty == dp[i][j])
        {
            xans[xpos--] = (int)x[i - 1];
            yans[ypos--] = (int)y[j - 1];
            i--; j--;
        }
        else if (dp[i - 1][j] + gap_penalty == dp[i][j])
        {
            xans[xpos--] = (int)x[i - 1];
            yans[ypos--] = gap_node;
            i--;
        }
        else if (dp[i][j - 1] + gap_penalty == dp[i][j])
        {
            xans[xpos--] = gap_node;
            yans[ypos--] = (int)y[j - 1];
            j--;
        }
    }
    while (xpos > 0)
    {
        if (i > 0) xans[xpos--] = (int)x[--i];
        else xans[xpos--] = gap_node;
    }
    while (ypos > 0)
    {
        if (j > 0) yans[ypos--] = (int)y[--j];
        else yans[ypos--] = gap_node;
    }
 
    // Since we have assumed the answer to be n+m long,
    // we need to remove the extra gaps in the starting
    // id represents the index from which the arrays
    // xans, yans are useful
    int id = 1;
    for (i = l; i >= 1; i--)
    {
        if ((char)yans[i] == gap_node && (char)xans[i] == gap_node)
        {
            id = i + 1;
            break;
        }
    }
 
    x = vector<uint>(xans.begin() + id, xans.end());
    y = vector<uint>(yans.begin() + id, yans.end());

    return;
}

void greedyUniform2(vector<uint> &x, vector<uint> &y, uint gap_node, uint difference_penalty, uint gap_penalty)
{
    // table for storing optimal substructure answers
    vector<vector<uint>> penalty_table;
    for (uint i = 0; i < y.size() + x.size() + 1; i++)
        penalty_table.push_back(vector<uint>(y.size() + x.size() + 1, 0));

    // initialising the table
    for (uint i = 0; i <= (y.size() + x.size()); i++)
    {
        penalty_table[i][0] = i * gap_penalty;
        penalty_table[0][i] = i * gap_penalty;
    }

    // calculating the minimum penalty
    for (uint i = 1; i <= x.size(); i++)
        for (uint j = 1; j <= y.size(); j++)
            if (x[i - 1] == y[j - 1])
                penalty_table[i][j] = penalty_table[i - 1][j - 1];
            else
                penalty_table[i][j] = std::min({penalty_table[i - 1][j - 1] + difference_penalty,
                                                penalty_table[i - 1][j] + gap_penalty,
                                                penalty_table[i][j - 1] + gap_penalty});

    // Reconstructing the solution
    int max_size = y.size() + x.size();

    int i = x.size();
    int j = y.size();

    int x_position = max_size;
    int y_position = max_size;

    // Final answers for the respective strings
    vector<uint> x_answer(max_size + 1, gap_node);
    vector<uint> y_answer(max_size + 1, gap_node);

    while (!(i == 0 || j == 0))
    {
        if (x[i - 1] == y[j - 1])
        {
            x_answer[x_position--] = x[i - 1];
            y_answer[y_position--] = y[j - 1];
            i--;
            j--;
        }
        else if (penalty_table[i - 1][j - 1] + difference_penalty == penalty_table[i][j])
        {
            x_answer[x_position--] = x[i - 1];
            y_answer[y_position--] = y[j - 1];
            i--;
            j--;
        }
        else if (penalty_table[i - 1][j] + gap_penalty == penalty_table[i][j])
        {
            x_answer[x_position--] = x[i - 1];
            y_answer[y_position--] = gap_node;
            i--;
        }
        else if (penalty_table[i][j - 1] + gap_penalty == penalty_table[i][j])
        {
            x_answer[x_position--] = gap_node;
            y_answer[y_position--] = y[j - 1];
            j--;
        }
    }
    while (x_position > 0)
    {
        x_answer[x_position--] = i > 0 ? x[--i] : gap_node;
    }
    while (y_position > 0)
    {
        y_answer[y_position--] = i > 0 ? x[--i] : gap_node;
    }

    // Since we have assumed the answer to be x.size + y.size long,
    // we need to remove the extra gaps in the starting
    // start represents the index from which the arrays
    // x_answer, y_answer are useful
    int start = 0;
    while (y_answer[start] == gap_node && x_answer[start] == gap_node)
    {
        start++;
    }
    
    x = vector<uint>(x_answer.begin() + start, x_answer.end());
    y = vector<uint>(y_answer.begin() + start, y_answer.end());
}

void alignment::greedyOneGap(vector<uint> &x, vector<uint> &y, uint gap_node)
{
    if (x.size() == y.size())
        return;
    int idx = 0;
    int alignment = 0;
    while (idx < x.size() && idx < y.size())
    {
        alignment += x[idx] == y[idx];
        idx++;
    }
    int best_alignment = alignment;
    int best_idx = idx;
    int x_idx = x.size() - 1;
    int y_idx = y.size() - 1;
    int gap_size = std::abs((int)x.size() - (int)y.size());

    while (std::min(x_idx, y_idx) >= 0)
    {
        alignment += x[x_idx] == y[y_idx];
        if (x.size() > y.size())
            alignment -= x[x_idx - gap_size] == y[y_idx];
        else
            alignment -= x[x_idx] == y[y_idx - gap_size];

        if (alignment > best_alignment)
        {
            best_alignment = alignment;
            best_idx = std::max(x_idx, y_idx) - gap_size;
        }
        x_idx--;
        y_idx--;
    }

    if (x.size() > y.size())
        y.insert(y.begin() + best_idx, gap_size, gap_node);
    else
        x.insert(x.begin() + best_idx, gap_size, gap_node);
}

void alignment::randomOneGap(vector<uint> &x, vector<uint> &y, uint gap_node, std::mt19937 *rng)
{
    if (x.size() < y.size())
    {
        std::uniform_int_distribution<uint> idx_rng(0, x.size());
        x.insert(x.begin() + idx_rng(*rng), y.size() - x.size(), gap_node);
    }
    else if (y.size() < x.size())
    {
        std::uniform_int_distribution<uint> idx_rng(0, y.size());
        y.insert(y.begin() + idx_rng(*rng), x.size() - y.size(), gap_node);
    }
}

void alignment::randomUniform(vector<uint> &x, vector<uint> &y, uint gap_node, std::mt19937 *rng)
{
    while (x.size() < y.size())
    {
        std::uniform_int_distribution<uint> idx_rng(0, x.size());
        x.insert(x.begin() + idx_rng(*rng), gap_node);
    }
    while (y.size() < x.size())
    {
        std::uniform_int_distribution<uint> idx_rng(0, y.size());
        y.insert(y.begin() + idx_rng(*rng), gap_node);
    }
}

void alignment::backfillOneGap(vector<uint> &x, vector<uint> &y, uint gap_node)
{
    if (x.size() < y.size())
        x.insert(x.end(), y.size() - x.size(), gap_node);

    if (y.size() < x.size())
        y.insert(y.end(), x.size() - y.size(), gap_node);
}

void alignment::frontfillOneGap(vector<uint> &x, vector<uint> &y, uint gap_node)
{
    if (x.size() < y.size())
        x.insert(x.begin(), y.size() - x.size(), gap_node);

    if (y.size() < x.size())
        y.insert(y.begin(), x.size() - y.size(), gap_node);
}
