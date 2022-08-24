//
// Created by wolledav on 24.8.22.
//

#include "tspsd.hpp"

uint TSPSDInstance::get_internal_id(const string& id) {
    return std::stoi(id) - 1;
}

string TSPSDInstance::get_original_id(uint id) {
    return std::to_string(id + 1);
}

TSPSDInstance::TSPSDInstance(const char *path) : Instance() {
    json data = read_json(path);

    // Fill Instance attributes
    this->type = data["TYPE"];
    this->name = data["NAME"];
    this->node_cnt = data["DIMENSION"];
    this->lbs = vector<uint>(this->node_cnt, 1);
    this->ubs = vector<uint>(this->node_cnt, 1);

    // Fill TSPSDInstance attributes
    this->positions.resize(this->node_cnt);                // coords
    for (const auto& item:data["NODE_COORDS"].items()) {
        uint id = get_internal_id(item.key());
        this->positions[id].x = item.value()[0];
        this->positions[id].y = item.value()[1];
    }
    this->dist_mat.resize(this->node_cnt, node_cnt);    // distance matrix
    compute_dist_mat();
    f_delete.resize(this->node_cnt);                       // delete function
    for (const auto& item:data["DELETE"].items()) {
        uint id = get_internal_id(item.key());
        for (const auto& val:item.value()) {
            f_delete[id].push_back(get_internal_id(val));
        }
    }
}

bool TSPSDInstance::compute_fitness(const vector<uint> &permutation, fitness_t *fitness) {
    *fitness = 0;
    // Initialize matrix for deleted edges
    boost::numeric::ublas::matrix<bool> deleted_mat(this->node_cnt, this->node_cnt, false);

    //
    for (uint i = 0; i < permutation.size(); i++) {
        *fitness += dist_mat(permutation[i], permutation[(i + 1) % permutation.size()]);
    }

    return false;
}

uint TSPSDInstance::compute_dist(uint id1, uint id2) {
    if (id1 == id2) { return 0; }
    uint xdiff = this->positions[id1].x - this->positions[id2].x;
    uint ydiff = this->positions[id1].y - this->positions[id2].y;
    double distance = sqrt(xdiff*xdiff + ydiff*ydiff);
    return (uint)round(distance);
}

void TSPSDInstance::compute_dist_mat() {
    for (uint i = 0; i < this->node_cnt; i++){
        for (uint j = 0; j <= i; j++){
            this->dist_mat(i, j) = this->compute_dist(i, j);
            this->dist_mat(j, i) = this->dist_mat(i, j);
        }
    }
}

