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

using boost::format;
using std::pair;
using std::vector;
using std::string;

// Metaheuristical optimizer for problems definable as Permutations With Repetitions
class LS_optimizer
{
    private:
        json config;
        uint timeout_s;
        std::chrono::steady_clock::time_point start, last_improvement;
        Instance* instance;
        Solution solution, best_known_solution;
        std::mt19937 *rng;    // random-number engine used (Mersenne-Twister)
        vector<string> operation_list;
        vector<string> perturbation_list;
        std::function<void()> construction;
        std::function<void()> metaheuristic;
        std::function<void()> local_search;
        std::basic_ostream<char> *log_stream = nullptr;
        vector<pair<long, fitness_t>> steps;
        const std::map<string, std::function<bool()>> operation_map = {
            {"insert_1",      [this](){return this->insert1();}},
            {"remove_1",      [this](){return this->remove1();}},
            {"two_opt",       [this](){return this->two_opt();}},
            {"exchange_ids",  [this](){return this->exchange_ids();}},
            {"exchange_n_ids",  [this](){return this->exchange_n_ids();}},
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
            {"centered_exchange_1",    [this](){return this->centered_exchange(1);}},
            {"centered_exchange_2",    [this](){return this->centered_exchange(2);}},
            {"centered_exchange_3",    [this](){return this->centered_exchange(3);}},
            {"centered_exchange_4",    [this](){return this->centered_exchange(4);}},
            {"centered_exchange_5",    [this](){return this->centered_exchange(5);}},
            {"relocate_1",    [this](){return this->relocate(1, false);}},
            {"relocate_2",    [this](){return this->relocate(2, false);}},
            {"relocate_3",    [this](){return this->relocate(3, false);}},
            {"relocate_4",    [this](){return this->relocate(4, false);}},
            {"relocate_5",    [this](){return this->relocate(5, false);}},
            {"rrelocate_2",   [this](){return this->relocate(2, true);}},
            {"rrelocate_3",   [this](){return this->relocate(3, true);}},
            {"rrelocate_4",   [this](){return this->relocate(4, true);}},
            {"rrelocate_5",   [this](){return this->relocate(5, true);}},
            {"move_all_by_1",    [this](){return this->move_all(1);}},
            {"move_all_by_2",    [this](){return this->move_all(2);}},
            {"move_all_by_3",    [this](){return this->move_all(3);}},
            {"move_all_by_4",    [this](){return this->move_all(4);}},
            {"move_all_by_10",    [this](){return this->move_all(10);}},
        };
        const std::map<string, std::function<void(uint)>> perturbation_map = {
            {"double_bridge",        [this](uint k){this->double_bridge(k, true);}},
            {"random_double_bridge", [this](uint k){this->double_bridge(k, false);}},
            {"reinsert",             [this](uint k){this->reinsert(k);}},
            {"random_move_all",      [this](uint k){this->random_move_all(k);}},
            {"random_move",          [this](uint k){this->random_move(k);}},
            {"random_swap",          [this](uint k){this->random_swap(k);}}
        };
        // Initial solution
        void construct_greedy();
        void construct_random();
        void construct_random_replicate();
        // Operations
        bool insert1();
        bool remove1();
        bool relocate(uint x, bool reverse=false);
        bool exchange(uint x, uint y, bool reverse=false);
        bool centered_exchange(uint x);
        bool move_all(uint x);
        bool exchange_ids();
        bool exchange_n_ids();
        bool two_opt();
        // Perturbations
        void double_bridge(uint k, bool reverse_all);
        void reinsert(uint k);
        void random_swap(uint k);
        void random_move(uint k);
        void random_move_all(uint k);
        // Local Search
        void basicVND();
        void pipeVND();
        void cyclicVND();
        void randomVND();
        void randompipeVND();
        // Metaheuristics
        void ILS();
        void basicVNS();
        void calibratedVNS();
        // utils
        void perturbation_call(const string &perturbation_name, uint k);
        bool operation_call(const string &operation_name);
        long get_runtime();
        bool timeout();
        void random_reverse(std::vector<uint>::iterator, std::vector<uint>::iterator);
        Solution make_solution(const vector<uint> &permutation);
        bool valid_solution(Solution *sol);
        void print_operation(bool update, const string& msg);
    public:
        std::map<string, std::pair<uint, uint>> operation_histogram = {};
        // Initialization
        explicit LS_optimizer(Instance* inst, json config, uint seed=0);
        ~LS_optimizer() = default;
        static std::mt19937* init_rng(uint seed);
        void setLogger(std::basic_ostream<char>& logs);
        void setConstruction(const string& constr);
        void setMetaheuristic(const string& meta);
        void setLocalSearch(const string& loc_method);
        void setOperators(const vector<string>& operations);
        void setPerturbations(const vector<string>& perturbations);
        void run();
        // output
        Solution getSolution() {return this->best_known_solution;}
        void save_to_json(json& container);
};
