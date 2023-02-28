#pragma once

#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <boost/format.hpp>
#include <random>
#include <chrono>
#include <stdexcept>
#include <omp.h>
#include <numeric>
#include <unordered_set>

#include "instance.hpp"
#include "solution.hpp"
#include "utils.hpp"
#include "basic_optimizer.hpp"

inline void LOGS(const std::vector<Solution> parents) {for (auto p : parents) p.print();}


class EA : public BasicOptimizer
{
    private:
        uint timeout_s;
        uint unimproved_cnt;
        std::vector<Solution> population;
        uint population_size;
        double t_target = 0.5;
        std::vector<double> t_t;
        double t_select = 0.3;
        std::vector<double> penalty_coefficients;
        std::mt19937 *rng;
        static std::mt19937* initRng(uint seed);
        void update_t_t();
        void update_penalty_coefficients();
        permutator::fitness_t penalized_fitness(Solution solution);
        permutator::fitness_t penalized_fitness(std::vector<permutator::fitness_t> penalties);
        void sort_by_pf(std::vector<Solution> &v);

        std::function<Solution()> construction;
        std::function<void(std::vector<Solution> &parents)> selection;
        std::function<void(std::vector<Solution> parents, std::vector<Solution> &children)> crossover;
        std::function<void(Solution &child)> mutation;
        std::function<void(std::vector<Solution> children)> replacement;

        std::vector<std::string> mutationList;

        const std::map<std::string, std::function<void(Solution &childe)>> mutation_map = {
            {"insert",      [this](Solution &childe){return this->insert(childe);}},
            {"centeredExchange_3",      [this](Solution &childe){return this->centeredExchange(3, childe);}},
            {"relocate_1",      [this](Solution &childe){return this->relocate(1, false, childe);}},
            {"remove",      [this](Solution &childe){return this->remove(childe);}},
        };

        //construction
        Solution constructRandom();
        Solution constructRandomReplicate();
        //selection
        void constrainedTournament(std::vector<Solution> &parents);
        //crossover
        void swapNode(std::vector<Solution> parents, std::vector<Solution> &children);
        void ERX(std::vector<Solution> parents, std::vector<Solution> &children);
        //mutation
        void insert(Solution &child);
        void centeredExchange(uint x, Solution &child); 
        void relocate(uint x, bool reverse, Solution &child);
        void remove(Solution &child);
        //replacement
        void segregational(std::vector<Solution> children);
        
        


    public:
        explicit EA(Instance* inst, nlohmann::json config, uint seed=0);
        ~EA() = default;
        void setConstruction(const std::string &constr);
        void setSelection(const std::string &constr);
        void setCrossover(const std::string &constr);
        void setMutation(const std::vector<std::string>& constr);
        void setReplacement(const std::string &constr);
        void run();
        void saveToJson(nlohmann::json& container);
};