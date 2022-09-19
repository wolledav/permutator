//
// Created by wolledav on 24.8.22.
//

#include "wcp.hpp"

uint WCPInstance::get_internal_id(const string& id) {
    return std::stoi(id) - 1;
}

string WCPInstance::get_original_id(uint id) {
    return std::to_string(id + 1);
}

void WCPInstance::export_perm_orig_ids(vector<uint> &perm, json &container) {
    vector<string> perm_orig_ids;
    for (auto node:perm) {
        perm_orig_ids.push_back(get_original_id(node));
    }
    container["solution"]["permutation_orig_ids"] = perm_orig_ids;
}

WCPInstance::WCPInstance(const char *path) : Instance() {
    json data = read_json(path);

    // Fill Instance attributes
    this->type = "wcp";
    this->name = data["NAME"];
    this->node_cnt = data["DIMENSION"];
    this->lbs = vector<uint>(this->node_cnt, 1);
    this->ubs = vector<uint>(this->node_cnt, 1);

    // Fill WCPInstance attributes
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
            auto del_edge = std::make_pair(get_internal_id(val[0]), get_internal_id(val[1]));
            f_delete[id].push_back(del_edge);
        }
    }
}

bool WCPInstance::compute_fitness(const vector<uint> &permutation, fitness_t *fitness) {
    *fitness = 0;
    bool valid = true;

    // Initialize matrix for deleted edges
    boost::numeric::ublas::matrix<bool> del_mat(this->node_cnt, this->node_cnt, false);
    boost::numeric::ublas::matrix<fitness_t> dist_mat_updated = dist_mat;

    for (uint i = 0; i < permutation.size(); i++) {
        uint node1 = permutation[i];
        uint node2 = permutation[(i + 1) % permutation.size()];

        // Process node1
        for (auto del:this->f_delete[node1]) {
            del_mat(del.first, del.second) = true;
            del_mat(del.second, del.first) = true;
            dist_mat_updated(del.first, del.second) = DELETED_EDGE_PENALTY;
            dist_mat_updated(del.second, del.first) = DELETED_EDGE_PENALTY;
        }

        // Update fitness and validity
        vector<uint> path{};
        *fitness += dijkstra(dist_mat_updated, node1, node2, path);
        for (uint j = 0; j < path.size() - 1; j++) {
            valid = valid && !del_mat(path[j], path[j + 1]);
        }
    }

    return valid;
}

uint WCPInstance::compute_dist(uint id1, uint id2) {
    if (id1 == id2) { return 0; }
    uint xdiff = this->positions[id1].x - this->positions[id2].x;
    uint ydiff = this->positions[id1].y - this->positions[id2].y;
    double distance = sqrt(xdiff*xdiff + ydiff*ydiff);
    return (uint)round(distance);
}

void WCPInstance::compute_dist_mat() {
    for (uint i = 0; i < this->node_cnt; i++){
        for (uint j = 0; j <= i; j++){
            this->dist_mat(i, j) = this->compute_dist(i, j);
            this->dist_mat(j, i) = this->dist_mat(i, j);
        }
    }
}

uint WCPInstance::closest_unused(const vector<fitness_t>& dist, const vector<bool>& sptSet) {
    fitness_t min = std::numeric_limits<fitness_t>::max()/2;
    uint min_index = std::numeric_limits<uint>::max()/2;

    for (uint v = 0; v < dist.size(); v++){
        if (!sptSet[v] && dist[v] <= min) {
            min = dist[v], min_index = v;
        }
    }

    return min_index;
}

fitness_t WCPInstance::dijkstra(const boost::numeric::ublas::matrix<fitness_t>& graph, uint start, uint goal, vector<uint> &path) {
    auto size = graph.size1();
    vector<fitness_t> dist(size, std::numeric_limits<fitness_t>::max()/2);
    vector<bool> sptSet(size, false);
    vector<uint> prev(size, start);

    dist[start] = 0;
    for (uint count = 0; count < size - 1; count++) {
        auto u = closest_unused(dist, sptSet);
        sptSet[u] = true;
        for (uint v = 0; v < size; v++)
            if (!sptSet[v] && dist[u] != std::numeric_limits<fitness_t>::max()/2 && dist[u] + graph(u, v) < dist[v]) {
                dist[v] = dist[u] + graph(u, v);
                prev[v] = u;
            }
    }

    auto current = goal;
    do {
        path.push_back(current);
        current = prev[current];
    } while (current != start);
    path.push_back(current);
    reverse(path.begin(), path.end());

    return dist[goal];
}

void WCPInstance::export_walk_orig_ids(vector<uint> &permutation, json &container) {
    vector<string> walk_orig_ids;
    vector<bool> walk_processing;

    // Initialize matrix for deleted edges
    boost::numeric::ublas::matrix<bool> del_mat(this->node_cnt, this->node_cnt, false);
    boost::numeric::ublas::matrix<fitness_t> dist_mat_updated = dist_mat;

    for (uint i = 0; i < permutation.size(); i++) {
        uint node1 = permutation[i];
        uint node2 = permutation[(i + 1) % permutation.size()];

        // Process node1
        for (auto del:this->f_delete[node1]) {
            del_mat(del.first, del.second) = true;
            del_mat(del.second, del.first) = true;
            dist_mat_updated(del.first, del.second) = DELETED_EDGE_PENALTY;
            dist_mat_updated(del.second, del.first) = DELETED_EDGE_PENALTY;
        }

        // Update fitness and validity
        vector<uint> path{};
        dijkstra(dist_mat_updated, node1, node2, path);

        for (uint j = 0; j < path.size() - 1; j++) {
            walk_orig_ids.push_back(get_original_id(path[j]));
            j == 0 ? walk_processing.push_back(true) : walk_processing.push_back(false);
        }
    }

    container["solution"]["walk_orig_ids"] = walk_orig_ids;
    container["solution"]["walk_processing"] = walk_processing;
}





