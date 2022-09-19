#pragma once

#include <cstdio>
#include <limits>
#include <utility>
#include <vector>

#include "../config.hpp"
#include "../utils.hpp"

using std::vector;
using namespace metaopt;

class Instance
{
    protected:
        Instance(const string& name, uint node_count);
        Instance(const string& name, uint node_count, uint common_lb, uint common_ub);
        Instance() = default;
    //explicit Instance(uint node_count);
        ~Instance() = default;
    public:
        vector<uint> lbs, ubs;
        uint node_cnt{};
        string name, type = "no-type";
        virtual bool compute_fitness(const vector<uint> &permutation, fitness_t* fitness) = 0;
        fitness_t get_lb_penalty(const vector<uint> &frequency);
        bool frequency_in_bounds(const vector<uint> &frequency);

};