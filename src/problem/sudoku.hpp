//
// Created by honza on 11.03.22.
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
using boost::numeric::ublas::matrix;

class SudokuInstance : public Instance {
private:
public:
    uint subdivision, fixed_cnt;
    matrix<int> grid;
    string type = "sudoku";

    SudokuInstance(const string& path, uint nodes);
    ~SudokuInstance() = default;
    void fill_grid(matrix<int>& target_grid, const vector<uint> &permutation);
    bool compute_fitness(const vector<uint> &permutation, fitness_t* fitness) override;
    void read(const char* path);
    void print_solution(Solution *solution, std::basic_ostream<char>& outf = std::cout) const;
    void print_nodes();
};
