//
// Created by honza on 21.11.21.
//

#ifndef METAOPT_SOLUTION_H
#define METAOPT_SOLUTION_H

#include <vector>
#include <limits>

#include "../config.hpp"
#include "../utils.hpp"
#include "instance.hpp"

using std::vector;
using namespace metaopt;

class Solution
{
public:
    fitness_t fitness;
    bool is_feasible = false;
    vector<uint> permutation;
    vector<uint> frequency;

    // Constructors
    Solution();
    explicit Solution(uint node_cnt);
    Solution(uint node_cnt, std::vector<uint> &freq);
    Solution(uint node_cnt, vector<uint> &perm, fitness_t fit, bool is_feasible);
    Solution(vector<uint> &perm, Instance &instance);

    // Utils
    uint getSize() const {return this->permutation.size();}
    void save_to_json(json& obj);
    void print();

    // operator overloads
    bool operator== (const Solution& other);
    bool operator<= (const Solution& other) const;
    bool operator< (const Solution& other) const;
    bool operator> (const Solution& other) const;
    bool operator>= (const Solution& other) const;
    bool operator[] (uint idx);
};

#endif //METAOPT_SOLUTION_H
