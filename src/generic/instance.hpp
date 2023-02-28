#pragma once

#include <cstdio>
#include <limits>
#include <utility>
#include <vector>
#include <iostream>

#include "../config.hpp"
#include "../utils.hpp"
#include "lib/getopt/getopt.h"

class Instance
{
    protected:
        Instance(const std::string& name, uint node_count);
        Instance(const std::string& name, uint node_count, uint common_lb, uint common_ub);
        Instance() = default;
        ~Instance() = default;

    public:
        std::vector<uint> lbs, ubs;
        uint node_cnt{};
        std::string name, type = "no-type";
        uint fitness_evals = 0;
        uint penalty_func_cnt{};

        virtual bool computeFitness(const std::vector<uint> &permutation, permutator::fitness_t &fitness, std::vector<permutator::fitness_t> &penalties) = 0;
        permutator::fitness_t getLBPenalty(const std::vector<uint> &frequency);
        bool FrequencyInBounds(const std::vector<uint> &frequency);
        static void parseArgs(int argc, char *argv[], std::string &data_path, std::string &output_path, std::string &optimizer_type, nlohmann::json &config, uint& seed, std::vector<uint> *init_solution = nullptr);
        static void show_usage(){ std::cout << "Usage: XXXX_meta -d data_path [-c] config_path [-t] timeout(sec) [-s] seed [-o] output_file_path\n";}

};