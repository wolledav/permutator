#include "instance.hpp"

using namespace permutator;
using std::string;

Instance::Instance(const string& name, uint node_count) {
    this->name = name;
    this->node_cnt = node_count;
    this->lbs.resize(node_count);
    this->ubs.resize(node_count);
    for (uint i = 0; i < node_count; i++) {
        this->lbs[i] = 0;
        this->ubs[i] = std::numeric_limits<uint>::max()/2;
    }
}

Instance::Instance(const string& name, uint node_count, uint common_lb, uint common_ub) {
    this->name = name;
    this->node_cnt = node_count;
    this->lbs.resize(node_count);
    this->ubs.resize(node_count);
    for (uint i = 0; i < node_count; i++) {
        this->lbs[i] = common_lb;
        this->ubs[i] = common_ub;
    }
}

fitness_t Instance::get_lb_penalty(const std::vector<uint> &frequency) {
    uint sum = 0;
    for (uint i = 0; i < this->node_cnt; i++) {
        sum += std::max((int)this->lbs[i] - (int)frequency[i], 0);
    }
    return sum * LB_PENALTY;
}

bool Instance::frequency_in_bounds(const std::vector<uint> &frequency) {
    for (uint i = 0; i < this->node_cnt; i++){
        if (frequency[i] < this->lbs[i] || frequency[i] > this->ubs[i])
            return false;
    }
    return true;
}

