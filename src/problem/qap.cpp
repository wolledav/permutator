#include "qap.hpp"

using std::vector;

QAPInstance::QAPInstance(const char *path, uint count)
 : Instance(generate_name(path, "qap"), count, 1, 1){
    this->dist_mat.resize(count, count);
    this->flow_mat.resize(count, count);
    read(path);
}

void QAPInstance::read(const char *path) {
    std::ifstream ifs(path, std::ifstream::in);
    if (!ifs.good())
        throw std::system_error(ENOENT, std::system_category(), path);
    ifs >> this->problem_size;
    ifs >> this->bks_fitness;
    for (uint i = 0; i < this->node_cnt; i++)
        for (uint j = 0; j < this->node_cnt; j++)
            ifs >> flow_mat(i, j);
    for (uint i = 0; i < this->node_cnt; i++)
        for (uint j = 0; j < this->node_cnt; j++)
            ifs >> dist_mat(i, j);
}

bool QAPInstance::compute_fitness(const vector<uint> &permutation, permutator::fitness_t *fitness) {
    *fitness = 0;
    if (permutation.size() == this->node_cnt) {
        for (uint i = 0; i < this->node_cnt; i++) {
            for (uint j = 0; j < this->node_cnt; j++) {
                *fitness += this->flow_mat(i, j)*this->dist_mat(permutation[i], permutation[j]);
            }
        }
        return true;
    } else {
        *fitness += (this->node_cnt - permutation.size()) * JOB_MISSING_PENALTY;
        return false;
    }
}

void QAPInstance::print_solution(Solution *solution, std::basic_ostream<char> &outf) {
    outf << "Fitness: " << solution->fitness << "\nPermutation:\n";
    for (uint i = 0; i < this->node_cnt; i++)
        outf << solution->permutation[i] + 1 << " ";
    outf << std::endl;
}

void QAPInstance::print_nodes() {
    std::cout << "Flow matrix\n";
    std::cout << "\t";
    for (uint i = 1; i <= this->node_cnt; i++)
        std::cout << std::setw(4)  << i;
    std::cout << std::endl;
    for (uint i = 0; i < this->node_cnt; i++){
        std::cout << i+1 << "\t";
        for (uint j = 0; j < this->node_cnt; j++) {
            std::cout << std::setw(4) << this->flow_mat(i, j);
        }
        std::cout << std::endl;
    }

    std::cout << "Distance matrix\n";
    std::cout << "\t";
    for (uint i = 1; i <= this->node_cnt; i++)
        std::cout << std::setw(4)  << i;
    std::cout << std::endl;
    for (uint i = 0; i < this->node_cnt; i++){
        std::cout << i+1 << "\t";
        for (uint j = 0; j < this->node_cnt; j++) {
            std::cout << std::setw(4) << this->dist_mat(i, j);
        }
        std::cout << std::endl;
    }
}
