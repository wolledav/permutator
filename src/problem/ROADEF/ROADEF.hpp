#pragma once

#include <string>

#include "utils.hpp"
#include "generic/instance.hpp"
#include "generic/solution.hpp"
#include "code/parser.hpp"
#include "code/instance.hpp"

#define WORKLOAD_PENALTY  1000000
#define EXCLUSION_PENALTY 1000000

class ROADEFInstance : public Instance
{
private:
    ROADEF::Instance instance;
    ROADEF::Solution solution;
    void scheduleStartTimes(const std::vector<uint> &permutation);

public:
    std::string type = "ROADEF";
    std::string name;
    

    ROADEFInstance(const char *path);
    ~ROADEFInstance() = default;
    bool computeFitness(const std::vector<uint> &permutation, permutator::fitness_t &fitness, std::vector<permutator::fitness_t> &penalties) override;
    void parseDataFrom(const char *path);
    void print_nodes();
    void print_solution(Solution *solution, std::basic_ostream<char> &outf = std::cout);
};