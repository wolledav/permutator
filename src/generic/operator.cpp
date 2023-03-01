
#include "operator.hpp"

using std::vector;

void oprtr::insert(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> upperBounds, uint node, uint position)
{
    if (frequency[node] < upperBounds[node] && position <= permutation.size())
    {
        permutation.insert(permutation.begin() + position, 0);
        permutation[position] = node;
        frequency[node]++;
    }
}

void oprtr::append(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> upperBounds, uint node)
{
    if (frequency[node] < upperBounds[node])
    {
        permutation.emplace_back(node);
        frequency[node]++;
    }
}

void oprtr::remove(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lowerBounds, uint position)
{
    if (position < permutation.size() && frequency[permutation[position]] > lowerBounds[permutation[position]])
    {
        frequency[permutation[position]]--;
        permutation.erase(permutation.begin() + position);
    }
}

void oprtr::relocate(std::vector<uint> &permutation, uint from, uint to, uint length, bool reverse)
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

void oprtr::centeredExchange(std::vector<uint> &permutation, uint center, uint radius)
{

    uint position = center - radius;
    relocate(permutation, position, position, 2 * radius + 1, true);
}

void oprtr::exchange(std::vector<uint> &permutation, uint from, uint to, uint sizeX, uint sizeY, bool reverse)
{

    if ((from >= to && from < to + sizeY) || (to >= from && to < from + sizeX))
        return;

    vector<uint> subx, suby;
    if (reverse)
    {
        subx.insert(subx.begin(), permutation.rend() - from - sizeX, permutation.rend() - from);
        suby.insert(suby.begin(), permutation.rend() - to - sizeY, permutation.rend() - to);
    }
    else
    {
        subx.insert(subx.begin(), permutation.begin() + from, permutation.begin() + from + sizeX);
        suby.insert(suby.begin(), permutation.begin() + to, permutation.begin() + to + sizeY);
    }
    if (from < to)
    {
        permutation.erase(permutation.begin() + to, permutation.begin() + to + sizeY);
        permutation.insert(permutation.begin() + to, subx.begin(), subx.end());
        permutation.erase(permutation.begin() + from, permutation.begin() + from + sizeX);
        permutation.insert(permutation.begin() + from, suby.begin(), suby.end());
    }
    else
    {
        permutation.erase(permutation.begin() + from, permutation.begin() + from + sizeX);
        permutation.insert(permutation.begin() + from, suby.begin(), suby.end());
        permutation.erase(permutation.begin() + to, permutation.begin() + to + sizeY);
        permutation.insert(permutation.begin() + to, subx.begin(), subx.end());
    }
}

void oprtr::moveAll(std::vector<uint> &permutation, uint node, uint offset, std::vector<uint> *positions)
{

    if (offset == 0)
        return;

    std::vector<uint> *new_positions = positions == nullptr ? new vector<uint>(0) : positions;
    if (positions == nullptr)
    {
        for (uint i = 0; i < permutation.size(); i++)
            if (permutation[i] == node)
            {
                (*new_positions).push_back(i);
            }
    }

    for (auto &pos : *new_positions)
    {                                    // for all positions of node_id
        int new_pos = (int)pos + offset; // shift by i
        if (new_pos < 0)
            new_pos = (int)permutation.size() + new_pos; // too far left -> overflow from right end
        else if ((uint)new_pos > permutation.size() - 1)
            new_pos = new_pos - (int)permutation.size(); // too far right -> overflow from left end
        permutation.erase(permutation.begin() + pos);
        permutation.insert(permutation.begin() + new_pos, node);
        pos = new_pos;
    }
}

void oprtr::exchangeIds(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lowerBounds, std::vector<uint> upperBounds, uint nodeX, uint nodeY)
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

void oprtr::exchangeNIds(std::vector<uint> &permutation, std::vector<uint> &frequency, std::vector<uint> lowerBounds, std::vector<uint> upperBounds, uint nodeX, uint nodeY, uint maxSwap)
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

void oprtr::twoOpt(std::vector<uint> &permutation, uint position, uint length)
{
    if (position + length > permutation.size())
        return;
    reverse(permutation.begin() + position, permutation.begin() + position + length);
}
