#ifndef OBJECTIVE_H
#define OBJECTIVE_H

#include <iostream>
#include "types.hpp"

using namespace std;

class Objective {
    public:
        fitness_t_ mean_risk;
        fitness_t_ expected_excess;
        fitness_t_ final_objective;
        fitness_t_ total_resource_use;
        fitness_t_ workload_underuse;
        fitness_t_ workload_overuse;
        uint_t exclusion_penalty;
        fitness_t_ extended_objective;
        Objective();
        void print_state();
        bool is_valid();
};

#endif
