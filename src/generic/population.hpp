#pragma once

#include "solution.hpp"
#include "utils.hpp"

#define INVALID_SOLUTION_FITNESS ULONG_MAX
#define GROWTH_COEFFICIENT 1.1

using permutator::fitness_t;

class Population
{
    private:
        uint size;
        std::vector<Solution> last_gen_solutions;
        uint penalty_func_cnt;        
        fitness_t penalty_ceiling;
        std::vector<double> penalty = {};
        std::vector<double> t_t;
        double t_target = 0.5;

        void updatePenalty();
        void update_t_t();
        void updateAvgFitness();
        void stallPopulationCheck();
        
    public:
        bool is_stalled = false;
        uint generation = 0;
        uint niche_radius = 5;
        std::vector<Solution> solutions = {};
        fitness_t avg_fitness = INVALID_SOLUTION_FITNESS;
        Solution best_known_solution;

        Population(uint size, uint penalty_func_cnt, double t_target);
        Population() {};
        ~Population() = default;
        uint getMaxSize();
        uint getRealSize();
        std::vector<double> get_penalty_coefficients();
        std::vector<double> get_t_t();
        double get_t_target();

        void initializePenalty();
        void append(Solution solution);
        void clear();
        void update(); 
        void print();
};
