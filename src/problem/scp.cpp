//
// Created by wolledav on 24.8.22.
//

#include "scp.hpp"

uint SCPInstance::get_internal_id(const string &id) {
    return std::stoi(id) - 1;
}

string SCPInstance::get_original_id(uint id) {
    return std::to_string(id + 1);
}

void SCPInstance::export_perm_orig_ids(vector<uint> &perm, json &container) {
    vector<string> perm_orig_ids;
    for (auto node: perm) {
        perm_orig_ids.push_back(get_original_id(node));
    }
    container["solution"]["permutation_orig_ids"] = perm_orig_ids;
}

std::pair<uint, uint> get_edge(uint id1, uint id2) {
    return std::make_pair(std::min(id1, id2), std::max(id1, id2));
}

SCPInstance::SCPInstance(const char *path) : Instance() {
    json data = read_json(path);

    // Fill Instance attributes
    this->type = "scp";
    this->name = data["NAME"];
    this->node_cnt = data["DIMENSION"];
    this->lbs = vector<uint>(this->node_cnt, 1);
    this->ubs = vector<uint>(this->node_cnt, 1);

    // Fill SCPInstance attributes
    this->positions.resize(this->node_cnt);                // coords
    for (const auto &item: data["NODE_COORDS"].items()) {
        uint id = get_internal_id(item.key());
        this->positions[id].x = item.value()[0];
        this->positions[id].y = item.value()[1];
    }

    this->dist_mat.resize(this->node_cnt, node_cnt);    // distance matrix
    compute_dist_mat();

    f_delete.resize(this->node_cnt);                       // delete function
//    deleted_by.resize(this->node_cnt);
//    for (auto i = 0; i < this->node_cnt; i++) {
//        for (auto j = 0; j < this->node_cnt; j++) {
//            deleted_by(i, j) = std::vector<bool>(this->node_cnt, false);
//        }
//    }

    for (const auto &item: data["DELETE"].items()) {
        uint id = get_internal_id(item.key());
        for (const auto &val: item.value()) {
            auto id1 = get_internal_id(val[0]);
            auto id2 = get_internal_id(val[1]);
            auto del_edge = get_edge(id1, id2);
            f_delete[id].push_back(del_edge);

//            deleted_by(del_edge.first, del_edge.second)[id] = true;
        }
    }
}

bool SCPInstance::compute_fitness(const vector<uint> &permutation, fitness_t *fitness) {
    *fitness = 0;
    bool valid = true;

    if (permutation.size() > 1) {
        // Initialize matrix for deleted edges
        boost::numeric::ublas::matrix<bool> del_mat(this->node_cnt, this->node_cnt, false);

        for (uint i = 0; i < permutation.size(); i++) {
            uint node1 = permutation[i];
            uint node2 = permutation[(i + 1) % permutation.size()];

            // Process node1
            for (auto del: this->f_delete[node1]) {
                del_mat(del.first, del.second) = true;
                del_mat(del.second, del.first) = true;
            }
            // Update fitness and validity
            *fitness +=
                    del_mat(node1, node2) * DELETED_EDGE_PENALTY + (1 - del_mat(node1, node2)) * dist_mat(node1, node2);
            valid = valid && !del_mat(node1, node2);

            // Evaluate edge [node1, node2]
//            bool deleted = false;
//            auto edge = get_edge(node1, node2);
//
//            for (uint j = 0; j <= i; j++) { // look at nodes from 0 to i
//                deleted = deleted || deleted_by(edge.first, edge.second)[permutation[j]];
//            }
//            // Update fitness and validity
//            *fitness += deleted * DELETED_EDGE_PENALTY + (1 - deleted) * dist_mat(node1, node2);
//            valid = valid && !deleted;
        }
    }

    fitness_evals++;
    return valid;
}

uint SCPInstance::compute_dist(uint id1, uint id2) {
    if (id1 == id2) { return 0; }
    uint xdiff = this->positions[id1].x - this->positions[id2].x;
    uint ydiff = this->positions[id1].y - this->positions[id2].y;
    double distance = sqrt(xdiff * xdiff + ydiff * ydiff);
    return (uint) round(distance);
}

void SCPInstance::compute_dist_mat() {
    for (uint i = 0; i < this->node_cnt; i++) {
        for (uint j = 0; j <= i; j++) {
            this->dist_mat(i, j) = this->compute_dist(i, j);
            this->dist_mat(j, i) = this->dist_mat(i, j);
        }
    }
}




