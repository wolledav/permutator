//
// Created by honza on 05.12.21.
//

#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/format.hpp>
#include <iomanip>

#include "generic/instance.hpp"
#include "generic/solution.hpp"

using boost::format;

class NPFSInstance : public Instance {
private:
    uint job_duplication_penalty, empty_machine_penalty;
public:
    uint job_cnt, machine_cnt, big_m;
    boost::numeric::ublas::matrix<uint> job_mat;
    string type = "npfs";

    NPFSInstance(const string& path, uint jobs, uint machines);
    ~NPFSInstance() = default;
    bool compute_fitness(const vector<uint> &permutation, fitness_t* fitness) override;
    void read(const char* path);
    void print_nodes();
    void print_solution(Solution *solution, std::basic_ostream<char>& outf = std::cout) const;
};
