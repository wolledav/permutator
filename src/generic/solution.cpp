//
// Created by honza on 21.11.21.
//
#include <iostream>
#include "solution.hpp"

Solution::Solution() = default;

/*
 * Initialize an empty solution with frequencies set to zeros.
 */
Solution::Solution(uint node_cnt) {
    this->frequency.reserve(node_cnt);
    this->permutation.reserve(node_cnt);
    for (uint i = 0; i < node_cnt; i++)
        this->frequency.push_back(0);
    this->is_feasible = false;
    this->fitness = std::numeric_limits<fitness_t>::max()/2;
}

Solution::Solution(uint node_cnt, vector<uint> &perm, fitness_t fit, bool is_feasible) {
    this->frequency.reserve(node_cnt);
    this->permutation = std::move(perm);
    this->fitness = fit;
    this->is_feasible = is_feasible;
    for (auto node : this->permutation) {
        this->frequency[node]++;
    }
}

Solution::Solution(vector<uint> &perm, Instance &instance) {
    this->permutation = perm;
    this->frequency = vector<uint>(instance.node_cnt, 0);
    for (auto id:this->permutation) {
        this->frequency[id]++;
    }
    this->is_feasible = instance.compute_fitness(permutation, &this->fitness) &&
                               instance.frequency_in_bounds(this->frequency);

}

void Solution::save_to_json(json& container) {
    container["solution"]["fitness"] = this->fitness;
    container["solution"]["is_feasible"] = this->is_feasible;
    container["solution"]["permutation"] = this->permutation;
}

void Solution::print() {
    std::cout << "solution state" << std::endl;
    std::cout << "\tlength: " << permutation.size() << std::endl;
    std::cout << "\tfitness: " << fitness << std::endl;
    std::cout << "\tis_feasible: " << is_feasible << std::endl;

    std::cout << "\tpermutation: ";
    for (auto e:permutation) std::cout << e << " ";
    std::cout << std::endl;

    std::cout << "\tfrequency: ";
    for (auto e:frequency) std::cout << e << " ";
    std::cout << std::endl;
}

bool Solution::operator == (const Solution& other) {
    if (other.permutation.size() != this->permutation.size())
        return false;
    for (uint i = 0; i < other.permutation.size(); i++) {
        if (this->permutation[i] != other.permutation[i])
            return false;
    }
    return true;
}

bool Solution::operator <= (const Solution& other) const {
    return this->fitness <= other.fitness;
}

bool Solution::operator < (const Solution& other) const {
    return this->fitness < other.fitness;
}

bool Solution::operator > (const Solution& other) const {
    return this->fitness > other.fitness;
}

bool Solution::operator >= (const Solution& other) const {
    return this->fitness >= other.fitness;
}

bool Solution::operator[] (uint idx) {
    return this->permutation[idx];
}

