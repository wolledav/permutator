#include "instance.hpp"

Instance::Instance(const string& name, uint node_count) {
    this->name = name;
    this->node_cnt = node_count;
    this->lbs.resize(node_count);
    this->ubs.resize(node_count);
    for (uint i = 0; i < node_count; i++) {
        this->lbs[i] = 0;
        this->ubs[i] = std::numeric_limits<unsigned  int>::max();
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
