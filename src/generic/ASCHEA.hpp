#pragma once

#include <map>
#include <functional>
#include <iterator>
#include <algorithm>
#include <cmath>

#include "instance.hpp"
#include "solution.hpp"
#include "utils.hpp"
#include "basic_optimizer.hpp"
#include "operator.hpp"
#include "population.hpp"
#include "lib/nlohmann/json.hpp"

#define T_SELECT 0.3
#define POPULATION_FREQUENCY 4
#define MUTATION_RATE 0.8
#define POPULATIONS_MAX_CNT 32
#define STATIC_POPULATION "static"
#define DYNAMIC_POPULATION "dynamic"
#define DYNAMIC_POPULATION_SIZE 8
#define STATIC_POPULATION_SIZE 128
#define TOURNAMENT_SIZE 4


inline void LOGS(const std::vector<Solution> parents) {for (auto p : parents) p.print(); LOG(parents.size());}


class ASCHEA : public BasicOptimizer
{
    private:
        uint timeout_s;
        uint unimproved_cnt;
        Population* active_population;
        Population* best_population;
        std::vector<Population*> populations = std::vector<Population*>(POPULATIONS_MAX_CNT, nullptr);
        std::vector<uint> population_counter = std::vector<uint>(POPULATIONS_MAX_CNT, 0);
        
        double t_select = T_SELECT;
        uint population_frequency = POPULATION_FREQUENCY; 
        std::string population_type = DYNAMIC_POPULATION;
        double mutation_rate = MUTATION_RATE;     
        uint tournament_size = TOURNAMENT_SIZE;

        std::function<void()> construction;
        std::function<void(std::vector<Solution> &parents)> selection;
        void crossover(std::vector<Solution> parents, std::vector<Solution> &children);
        void mutation(std::vector<Solution> &children);
        std::function<void(std::vector<Solution> children)> replacement;
        std::function<void(std::vector<uint> &permutation_1, std::vector<uint> &permutation_2, uint gap_node)> alignment;

        std::vector<std::string> crossover_list;
        std::vector<std::string> mutation_list;

        const std::map<std::string, std::function<void(Solution &child)>> mutation_map = {
            {"insert_1",      [this](Solution &child){return this->insert(child);}},
            {"remove_1",      [this](Solution &child){return this->remove(child);}},
            {"relocate_1",      [this](Solution &child){return this->relocate(child, 1, false);}},
            {"relocate_2",      [this](Solution &child){return this->relocate(child, 2, false);}},
            {"relocate_3",      [this](Solution &child){return this->relocate(child, 3, false);}},
            {"relocate_4",      [this](Solution &child){return this->relocate(child, 4, false);}},
            {"relocate_5",      [this](Solution &child){return this->relocate(child, 5, false);}},
            {"revRelocate_2",      [this](Solution &child){return this->relocate(child, 2, true);}},
            {"revRelocate_3",      [this](Solution &child){return this->relocate(child, 3, true);}},
            {"revRelocate_4",      [this](Solution &child){return this->relocate(child, 4, true);}},
            {"revRelocate_5",      [this](Solution &child){return this->relocate(child, 5, true);}},
            {"centExchange_1",      [this](Solution &child){return this->centeredExchange(child, 1);}},
            {"centExchange_2",      [this](Solution &child){return this->centeredExchange(child, 2);}},
            {"centExchange_3",      [this](Solution &child){return this->centeredExchange(child, 3);}},
            {"centExchange_4",      [this](Solution &child){return this->centeredExchange(child, 4);}},
            {"centExchange_5",      [this](Solution &child){return this->centeredExchange(child, 5);}},
            {"exchange_1_1",      [this](Solution &child){return this->exchange(child, 1, 1, false);}},
            {"exchange_1_2",      [this](Solution &child){return this->exchange(child, 1, 2, false);}},
            {"exchange_2_2",      [this](Solution &child){return this->exchange(child, 2, 2, false);}},
            {"exchange_2_3",      [this](Solution &child){return this->exchange(child, 2, 3, false);}},
            {"exchange_2_4",      [this](Solution &child){return this->exchange(child, 2, 4, false);}},
            {"exchange_3_3",      [this](Solution &child){return this->exchange(child, 3, 3, false);}},
            {"exchange_3_4",      [this](Solution &child){return this->exchange(child, 3, 4, false);}},
            {"exchange_4_4",      [this](Solution &child){return this->exchange(child, 4, 4, false);}},
            {"revExchange_1_2",      [this](Solution &child){return this->exchange(child, 1, 2, true);}},
            {"revExchange_2_2",      [this](Solution &child){return this->exchange(child, 2, 2, true);}},
            {"revExchange_2_3",      [this](Solution &child){return this->exchange(child, 2, 3, true);}},
            {"revExchange_2_4",      [this](Solution &child){return this->exchange(child, 2, 4, true);}},
            {"revExchange_3_3",      [this](Solution &child){return this->exchange(child, 3, 3, true);}},
            {"revExchange_3_4",      [this](Solution &child){return this->exchange(child, 3, 4, true);}},
            {"revExchange_4_4",      [this](Solution &child){return this->exchange(child, 4, 4, true);}},
            {"moveAll_1",      [this](Solution &child){return this->moveAll(child, 1);}},
            {"moveAll_2",      [this](Solution &child){return this->moveAll(child, 2);}},
            {"moveAll_3",      [this](Solution &child){return this->moveAll(child, 3);}},
            {"moveAll_4",      [this](Solution &child){return this->moveAll(child, 4);}},
            {"moveAll_10",      [this](Solution &child){return this->moveAll(child, 10);}},
            {"exchangeIds",      [this](Solution &child){return this->exchangeIds(child);}},
            {"exchangeNIds",      [this](Solution &child){return this->exchangeNIds(child);}},
            {"twoOpt",      [this](Solution &child){return this->twoOpt(child);}}    
        };

        const std::map<std::string, std::function<void(Solution, Solution, Solution&)>> crossover_map = {
            {"NBX",     [this](Solution parent1, Solution parent2, Solution& child) { return this->NBX  (parent1,  parent2,  child); }},
            {"PBX",     [this](Solution parent1, Solution parent2, Solution& child) { return this->PBX  (parent1,  parent2,  child); }},
            {"ERX",     [this](Solution parent1, Solution parent2, Solution& child) { return this->ERX  (parent1,  parent2,  child); }},
            {"AEX",     [this](Solution parent1, Solution parent2, Solution& child) { return this->AEX  (parent1,  parent2,  child); }},
            {"OX",      [this](Solution parent1, Solution parent2, Solution& child) { return this->OX   (parent1,  parent2,  child); }},
            {"OBX",     [this](Solution parent1, Solution parent2, Solution& child) { return this->OBX  (parent1,  parent2,  child); }},
            {"CX",      [this](Solution parent1, Solution parent2, Solution& child) { return this->CX   (parent1,  parent2,  child); }},
            {"HGreX",   [this](Solution parent1, Solution parent2, Solution& child) { return this->HGreX(parent1,  parent2,  child); }},
            {"HRndX",   [this](Solution parent1, Solution parent2, Solution& child) { return this->HRndX(parent1,  parent2,  child); }},
            {"HProX",   [this](Solution parent1, Solution parent2, Solution& child) { return this->HProX(parent1,  parent2,  child); }},
            {"ULX",     [this](Solution parent1, Solution parent2, Solution& child) { return this->ULX  (parent1,  parent2,  child); }},
            {"RULX",    [this](Solution parent1, Solution parent2, Solution& child) { return this->RULX (parent1,  parent2,  child); }},
            {"EULX",    [this](Solution parent1, Solution parent2, Solution& child) { return this->EULX (parent1,  parent2,  child); }},
            {"ERULX",   [this](Solution parent1, Solution parent2, Solution& child) { return this->ERULX(parent1,  parent2,  child); }},
            {"UPMX",    [this](Solution parent1, Solution parent2, Solution& child) { return this->UPMX (parent1,  parent2,  child); }},
            {"SPX",     [this](Solution parent1, Solution parent2, Solution& child) { return this->SPX  (parent1,  parent2,  child); }},
            {"MPX",     [this](Solution parent1, Solution parent2, Solution& child) { return this->MPX  (parent1,  parent2,  child); }},
            {"APX",     [this](Solution parent1, Solution parent2, Solution& child) { return this->APX  (parent1,  parent2,  child); }}
        };

        //construction
        void constructRandom();
        void constructRandomReplicate();
        void constructGreedy();
        void constructNearestNeighbor();

        //selection
        void sortingTournament(std::vector<Solution> &parents);
        void presortedTournament(std::vector<Solution> &parents);

        //crossover
        void ERX(Solution parent1, Solution parent2, Solution& child);
        void AEX(Solution parent1, Solution parent2, Solution& child);
        void NBX(Solution parent1, Solution parent2, Solution& child);
        void PBX(Solution parent1, Solution parent2, Solution& child);
        void OX(Solution parent1, Solution parent2, Solution& child);
        void OBX(Solution parent1, Solution parent2, Solution& child);
        void CX(Solution parent1, Solution parent2, Solution& child);
        void HGreX(Solution parent1, Solution parent2, Solution& child);
        void HRndX(Solution parent1, Solution parent2, Solution& child);
        void HProX(Solution parent1, Solution parent2, Solution& child);
        void ULX(Solution parent1, Solution parent2, Solution& child);
        void RULX(Solution parent1, Solution parent2, Solution& child);
        void EULX(Solution parent1, Solution parent2, Solution& child);
        void ERULX(Solution parent1, Solution parent2, Solution& child);
        void UPMX(Solution parent1, Solution parent2, Solution& child);
        void SPX(Solution parent1, Solution parent2, Solution& child);
        void MPX(Solution parent1, Solution parent2, Solution& child);
        void APX(Solution parent1, Solution parent2, Solution& child);

        //mutation
        void insert(Solution &child);
        void append(Solution &child);
        void remove(Solution &child);
        void relocate(Solution &child, uint x, bool reverse);
        void centeredExchange(Solution &child, uint x); 
        void exchange(Solution &child, uint x, uint y, bool reverse);
        void moveAll(Solution &child, uint x);
        void exchangeIds(Solution &child);
        void exchangeNIds(Solution &child);
        void twoOpt(Solution &child);   

        //replacement
        void segregational(std::vector<Solution> children); 
        void nicheSegregational(std::vector<Solution> children);

        //utils
        static std::mt19937* initRng(uint seed);
        bool stop() override;
        permutator::fitness_t getFitness(std::vector<uint> permutation);
        permutator::fitness_t penalizedFitness(Solution solution);
        permutator::fitness_t penalizedFitness(std::vector<permutator::fitness_t> penalties);
        void sortByPenalizedFitness(std::vector<Solution> &v);
        void nichePopulation(std::vector<Solution> population, uint capacity, std::vector<bool> &leaders, std::vector<bool> &followers);
        void swapPopulation();
        void setPopulation(const std::string &constr);
        


    public:
        explicit ASCHEA(Instance* inst, nlohmann::json config, uint seed=0);
        ~ASCHEA();
        void setConstruction(const std::string &constr);
        void setSelection(const std::string &constr);
        void setCrossover(const std::string &constr);
        void setMutation(const std::vector<std::string>& constr);
        void setAlignment(const std::string &constr);
        void setReplacement(const std::string &constr);
        void run();
        void saveToJson(nlohmann::json& container);
};