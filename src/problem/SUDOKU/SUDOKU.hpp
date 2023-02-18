#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/format.hpp>
#include <iomanip>

#include "generic/instance.hpp"
#include "generic/solution.hpp"

class SudokuInstance : public Instance {
private:
public:
    uint subdivision, fixed_cnt;
    boost::numeric::ublas::matrix<int> grid;
    std::string type = "SUDOKU";

    SudokuInstance(const std::string& path, uint nodes);
    ~SudokuInstance() = default;
    void fill_grid(boost::numeric::ublas::matrix<int>& target_grid, const std::vector<uint> &permutation);
    bool computeFitness(const std::vector<uint> &permutation, permutator::fitness_t &fitness, std::vector<permutator::fitness_t> &penalties) override;
    void read(const char* path);
    void print_solution(Solution *solution, std::basic_ostream<char>& outf = std::cout) const;
    void print_nodes();
};
