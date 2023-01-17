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

#include "instance.hpp"
#include "solution.hpp"
#include "utils.hpp"


class Optimizer
{
    private:
        nlohmann::json config;
        uint timeout_s;
        std::chrono::steady_clock::time_point start, last_improvement;
        Instance* instance;
        Solution initial_solution, current_solution, best_known_solution;
        std::mt19937 *rng;
        std::vector<std::string> operation_list;
        std::vector<std::string> perturbation_list;
        std::function<void()> construction;
        std::function<void()> metaheuristic;
        std::function<void()> local_search;
        std::basic_ostream<char> *log_stream = nullptr;
        std::vector<std::pair<long, permutator::fitness_t>> steps;
        const std::map<std::string, std::function<bool()>> operation_map = {
            {"insert_1",      [this](){return this->insert1();}},
            {"remove_1",      [this](){return this->remove1();}},
            {"two_opt",       [this](){return this->twoOpt();}},
            {"exchange_ids",  [this](){return this->exchangeIds();}},
            {"exchange_n_ids",  [this](){return this->exchangeNIds();}},
            {"exchange_1_1",  [this](){return this->exchange(1, 1, false);}},
            {"exchange_1_2",  [this](){return this->exchange(1, 2, false);}},
            {"exchange_2_2",  [this](){return this->exchange(2, 2, false);}},
            {"exchange_2_3",  [this](){return this->exchange(2, 3, false);}},
            {"exchange_2_4",  [this](){return this->exchange(2, 4, false);}},
            {"exchange_3_3",  [this](){return this->exchange(3, 3, false);}},
            {"exchange_3_4",  [this](){return this->exchange(3, 4, false);}},
            {"exchange_4_4",  [this](){return this->exchange(4, 4, false);}},
            {"rexchange_1_2", [this](){return this->exchange(1, 2, true);}},
            {"rexchange_2_2", [this](){return this->exchange(2, 2, true);}},
            {"rexchange_2_3", [this](){return this->exchange(2, 3, true);}},
            {"rexchange_2_4", [this](){return this->exchange(2, 4, true);}},
            {"rexchange_3_3", [this](){return this->exchange(3, 3, true);}},
            {"rexchange_3_4", [this](){return this->exchange(3, 4, true);}},
            {"rexchange_4_4", [this](){return this->exchange(4, 4, true);}},
            {"centered_exchange_1",    [this](){return this->centeredExchange(1);}},
            {"centered_exchange_2",    [this](){return this->centeredExchange(2);}},
            {"centered_exchange_3",    [this](){return this->centeredExchange(3);}},
            {"centered_exchange_4",    [this](){return this->centeredExchange(4);}},
            {"centered_exchange_5",    [this](){return this->centeredExchange(5);}},
            {"relocate_1",    [this](){return this->relocate(1, false);}},
            {"relocate_2",    [this](){return this->relocate(2, false);}},
            {"relocate_3",    [this](){return this->relocate(3, false);}},
            {"relocate_4",    [this](){return this->relocate(4, false);}},
            {"relocate_5",    [this](){return this->relocate(5, false);}},
            {"rrelocate_2",   [this](){return this->relocate(2, true);}},
            {"rrelocate_3",   [this](){return this->relocate(3, true);}},
            {"rrelocate_4",   [this](){return this->relocate(4, true);}},
            {"rrelocate_5",   [this](){return this->relocate(5, true);}},
            {"move_all_by_1",    [this](){return this->moveAll(1);}},
            {"move_all_by_2",    [this](){return this->moveAll(2);}},
            {"move_all_by_3",    [this](){return this->moveAll(3);}},
            {"move_all_by_4",    [this](){return this->moveAll(4);}},
            {"move_all_by_10",    [this](){return this->moveAll(10);}},
        };
        const std::map<std::string, std::function<void(uint)>> perturbation_map = {
            {"doubleBridge",        [this](uint k){ this->doubleBridge(k, true);}},
            {"random_double_bridge", [this](uint k){ this->doubleBridge(k, false);}},
            {"reinsert",             [this](uint k){this->reinsert(k);}},
            {"randomMoveAll",      [this](uint k){ this->randomMoveAll(k);}},
            {"randomMove",          [this](uint k){ this->randomMove(k);}},
            {"randomSwap",          [this](uint k){ this->randomSwap(k);}}
        };
        // Initial solution constructions
        void constructGreedy();
        void constructRandom();
        void constructRandomReplicate();
        // Local search operators
        bool insert1();
        bool remove1();
        bool relocate(uint x, bool reverse=false);
        bool exchange(uint x, uint y, bool reverse=false);
        bool centeredExchange(uint x);
        bool moveAll(uint x);
        bool exchangeIds();
        bool exchangeNIds();
        bool twoOpt();
        // Perturbations
        void doubleBridge(uint k, bool reverse_all);
        void reinsert(uint k);
        void randomSwap(uint k);
        void randomMove(uint k);
        void randomMoveAll(uint k);
        // Local search heuristics
        void basicVND();
        void pipeVND();
        void cyclicVND();
        void randomVND();
        void randompipeVND();
        // Metaheuristics
        void ILS();
        void basicVNS();
        void calibratedVNS();
        // Utils
        void perturbationCall(const std::string &perturbation_name, uint k);
        bool operationCall(const std::string &operation_name);
        long getRuntime();
        bool timeout();
        void randomReverse(std::vector<uint>::iterator, std::vector<uint>::iterator);
        void printOperation(const std::string& msg);
        void printResult(bool update);
    public:
        std::map<std::string, std::pair<uint, uint>> operation_histogram = {};
        // Initialization
        explicit Optimizer(Instance* inst, nlohmann::json config, uint seed=0);
        ~Optimizer() = default;
        static std::mt19937* initRng(uint seed);
        void setLogger(std::basic_ostream<char>& logs);
        void setInitSolution(std::vector<uint> init_solution);
        void setConstruction(const std::string& constr);
        void setMetaheuristic(const std::string& meta);
        void setLocalSearch(const std::string& loc_method);
        void setOperators(const std::vector<std::string>& operations);
        void setPerturbations(const std::vector<std::string>& perturbations);
        void run();
        // Output
        Solution getSolution() {return this->best_known_solution;}
        void saveToJson(nlohmann::json& container);
};
