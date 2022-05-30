//
// Created by honza on 05.12.21.
//

#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <iomanip>

#include "generic/instance.hpp"
#include "generic/solution.hpp"

#define JOB_MISSING_PENALTY 1000000000

class QAPInstance : public Instance {
    public:
        uint problem_size, bks_fitness;
        boost::numeric::ublas::matrix<uint> dist_mat, flow_mat;
        string type = "qap";

        QAPInstance(const char* path, uint count);
        ~QAPInstance() = default;
        bool compute_fitness(const vector<uint> &permutation, fitness_t* fitness) override;
        void read(const char* path);
        void print_nodes();
        void print_solution(Solution *solution, std::basic_ostream<char>& outf = std::cout);
};
