#include <iostream>
#include "solution.hpp"

using permutator::fitness_t;
using nlohmann::json;

Solution::Solution() = default;

/*
 * Initialize an empty solution with frequencies set to zeros.
 * Fitness is set to max()/2.
 */
Solution::Solution(uint node_cnt) {
    this->frequency.reserve(node_cnt);
    this->permutation.reserve(node_cnt);
    for (uint i = 0; i < node_cnt; i++)
        this->frequency.push_back(0);
    this->is_feasible = false;
    this->fitness = std::numeric_limits<fitness_t>::max()/2;
}

/*
 * Initialize a solution based on given permutation.
 * Fitness is not evaluated from scratch.
 */
Solution::Solution(uint node_cnt, std::vector<uint> &perm, fitness_t fit, bool is_feasible) {
    this->frequency.reserve(node_cnt);
    this->permutation = std::move(perm);
    this->fitness = fit;
    this->is_feasible = is_feasible;
    for (auto node : this->permutation) {
        this->frequency[node]++;
    }
}

/*
 * Initialize a solution based on given permutation.
 */
Solution::Solution(std::vector<uint> &perm, Instance &instance) {
    this->permutation = perm;
    this->frequency = std::vector<uint>(instance.node_cnt, 0);
    for (auto id:this->permutation) {
        this->frequency[id]++;
    }
    this->is_feasible = instance.computeFitness(permutation, &this->fitness) &&
            instance.FrequencyInBounds(this->frequency);

}

void Solution::saveToJson(json& obj) {
    obj["solution"]["fitness"] = this->fitness;
    obj["solution"]["is_feasible"] = this->is_feasible;
    obj["solution"]["permutation"] = this->permutation;
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

