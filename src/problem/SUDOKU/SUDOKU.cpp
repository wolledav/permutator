#include "SUDOKU.hpp"

using std::vector;
using permutator::fitness_t;
using boost::format;
using boost::numeric::ublas::matrix;

SudokuInstance::SudokuInstance(const std::string& path, uint nodes)
        : Instance(generateName(path, "SUDOKU"), nodes, nodes, nodes) {
    this->grid.resize(nodes, nodes);
    this->read(path.c_str());
}

void SudokuInstance::read(const char *path) {
    int dummy;
    std::ifstream ifs(path, std::ifstream::in);
    if (!ifs.good())
        throw std::system_error(ENOENT, std::system_category(), path);
    ifs >> this->subdivision;
    ifs >> dummy;
    this->fixed_cnt = 0;
    for (uint i = 0; i < this->node_cnt; i++) {
        for (uint j = 0; j < this->node_cnt; j++) {
            ifs >> this->grid(i, j);
            if (this->grid(i, j) > 0) {
                this->grid(i, j)--; // 0 based
                this->fixed_cnt++;
                this->lbs[this->grid(i, j)]--;
                this->ubs[this->grid(i, j)]--;
            }
        }
    }
}

void SudokuInstance::fill_grid(matrix<int>& target_grid, const vector<uint> &permutation) {
    uint perm_idx = 0;
    for (uint i = 0; i < this->node_cnt; i++) {
        for (uint j = 0; j < this->node_cnt; j++) {
            if (target_grid(i, j) < 0)
                target_grid(i, j) = (int)permutation[perm_idx++];
            if (perm_idx >= permutation.size())
                return;
        }
    }
}

bool SudokuInstance::computeFitness(const vector<uint> &permutation, fitness_t* fitness) {
    uint si, sj, i, j;
    matrix<int> new_grid(this->grid);
    this->fill_grid(new_grid, permutation);
    *fitness = 0;
    // check vertical lines
    for (i = 0; i < this->node_cnt; i++) {
        vector<bool> is_present(this->node_cnt, false);
        for (j = 0; j < this->node_cnt; j++) {
            if (new_grid(j, i) < 0) {
                *fitness += 2;
            } else if (is_present[new_grid(j, i)]){
                *fitness += 1;
            } else {
                is_present[new_grid(j, i)] = true;
            }
        }
    }

    // check horizontal lines
    for (i = 0; i < this->node_cnt; i++) {
        vector<bool> is_present(this->node_cnt, false);
        for (j = 0; j < this->node_cnt; j++) {
            if (new_grid(i, j) < 0) {
                *fitness += 2;
            } else if (is_present[new_grid(i, j)]){
                *fitness += 1;
            } else {
                is_present[new_grid(i, j)] = true;
            }
        }
    }

    // check squares
    for (si = 0; si < this->subdivision*this->subdivision; si += this->subdivision) {
        for (sj = 0; sj < this->subdivision*this->subdivision; sj += this->subdivision) {
            vector<bool> is_present(this->node_cnt, false);
            for (i = 0; i < this->subdivision; i++) {
                for (j = 0; j < this->subdivision; j++){
                    if (new_grid(si+ i, sj + j) < 0) {
                        *fitness += 2;
                    } else if (is_present[new_grid(si+ i, sj + j)]){
                        *fitness += 1;
                    } else {
                        is_present[new_grid(si + i, sj + j)] = true;
                    }
                }
            }
        }
    }
    return *fitness == 0;
}

void SudokuInstance::print_solution(Solution *solution, std::basic_ostream<char> &outf) const {
    std::stringstream s;
    s << "Fitness: " << solution->fitness << "\nGrid:\n";
    uint perm_idx = 0;
    for (uint i = 0; i < this->node_cnt; i++) {
        for (uint j = 0; j < this->node_cnt; j++) {
            if (this->grid(i, j) >= 0) {
                s << std::setw(3) << this->grid(i, j) << " ";
            } else {
                s << std::setw(3) << solution->permutation[perm_idx++] << " ";
            }
        }
        s << std::endl;
    }
    s << std::endl;
    if (perm_idx != solution->permutation.size()) {
        std::cerr << "ERROR: Error during print_solution: perm_idx != permutation.size()\n";
        for (auto elem : solution->permutation) {
            outf << elem << " ";
        }
        outf << std::endl;
    } else {
        outf << s.str();
    }
}

void SudokuInstance::print_nodes() {
    for (uint i = 0; i < this->node_cnt; i++) {
        for (uint j = 0; j < this->node_cnt; j++) {
            if (this->grid(i, j)  < 0) {
                std::cout  << std::setw(3) << "-" << " ";
            } else {
                std::cout << std::setw(3) << this->grid(i, j) << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
