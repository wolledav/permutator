#pragma once

#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>

#include "solution.hpp"
#include "utils.hpp"

#define INVALID_SOLUTION_FITNESS ULONG_MAX

using permutator::fitness_t;

class Population
{
    private:
        std::vector<Solution> prevSolutions;
        uint penalty_func_cnt;        
        fitness_t penaltyLimit;
        std::vector<double> penalty_coefficients = {};
        std::vector<double> t_t;
        double t_target = 0.5;

        void update_penalty_coefficients();
        void update_t_t();
        void update_avg_fitness();
        void resetCheck();
        
    public:
        Population(uint size, uint penalty_func_cnt, double t_target);
        Population() {};
        ~Population() = default;
        uint size;
        bool is_stalled = false;
        uint generation = 0;
        uint nicheRadius = 5;
        std::vector<Solution> solutions = {};
        fitness_t avgFitness = INVALID_SOLUTION_FITNESS;
        Solution best_known_solution;

        
        uint getSize() {return this->solutions.size();};
        void append(Solution solution) {this->solutions.push_back(solution);};
        void clear() {this->solutions.clear();}
        void initPenaltyCoefs();
        void update(); 
        inline void print() { for (auto solution : this->solutions) solution.print(); std::cout << "popSize: " <<  this->size << " | popGen: " << this->generation << std::endl;};
        std::vector<double> get_penalty_coefficients() {return this->penalty_coefficients;};
        std::vector<double> get_t_t() {return this->t_t;};
        double get_t_target() {return this->t_target;};
};
