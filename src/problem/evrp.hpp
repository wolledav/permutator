#pragma once

#include "generic/instance.hpp"
#include <vector>

#define JOB_MISSING_PENALTY 100000000

class EVRPInstance: public Instance
{
    private:
        uint total_requests = 0;
    public:
        string type = "evrp";

        EVRPInstance(const char* path);
        ~EVRPInstance();
        bool compute_fitness(const vector<uint> &permutation, fitness_t* fitness) override;
};
