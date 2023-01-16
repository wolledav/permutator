#pragma once

#include <vector>
#include <limits>

#include "../config.hpp"
#include "../utils.hpp"
#include "instance.hpp"

class Solution
{
public:
    permutator::fitness_t fitness;
    bool is_feasible = false;
    std::vector<uint> permutation;
    std::vector<uint> frequency;

    // Constructors
    Solution();
    explicit Solution(uint node_cnt);
    Solution(std::vector<uint> &perm, Instance &instance);
    Solution(uint node_cnt, std::vector<uint> &perm, permutator::fitness_t fit, bool is_feasible);

    // Utils
    uint getSize() const {return this->permutation.size();}
    void save_to_json(nlohmann::json& obj);
    void print();

    // operator overloads
    bool operator== (const Solution& other);
    bool operator<= (const Solution& other) const;
    bool operator< (const Solution& other) const;
    bool operator> (const Solution& other) const;
    bool operator>= (const Solution& other) const;
    bool operator[] (uint idx);
};