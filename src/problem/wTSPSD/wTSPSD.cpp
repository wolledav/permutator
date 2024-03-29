#include "wTSPSD.hpp"

using std::vector;
using std::string;
using permutator::fitness_t;
using nlohmann::json;

uint wTSPSDInstance::get_internal_id(const string& id) {
    return std::stoi(id) - 1;
}

string wTSPSDInstance::get_original_id(uint id) {
    return std::to_string(id + 1);
}

void wTSPSDInstance::export_perm_orig_ids(vector<uint> &perm, json &container) {
    vector<string> perm_orig_ids;
    for (auto node:perm) {
        perm_orig_ids.push_back(get_original_id(node));
    }
    container["solution"]["permutation_orig_ids"] = perm_orig_ids;
}

wTSPSDInstance::wTSPSDInstance(const char *path) : Instance() {
    json data = readJson(path);

    // Fill Instance attributes
    this->type = "wTSPSD";
    this->name = data["NAME"];
    this->node_cnt = data["DIMENSION"];
    this->lbs = vector<uint>(this->node_cnt, 1);
    this->ubs = vector<uint>(this->node_cnt, 1);

    // Fill wTSPSDInstance attributes
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

bool wTSPSDInstance::computeFitness(const vector<uint> &permutation, fitness_t &fitness, vector<fitness_t> &penalties) {
    fitness = 0;
    bool valid = true;

    auto del_mat = boost::numeric::ublas::matrix<bool, boost::numeric::ublas::row_major>(this->node_cnt, this->node_cnt, false); // delete matrix used in compute_fitness

    for (uint i = 0; i < permutation.size(); i++) {
        uint node1 = permutation[i];
        uint node2 = permutation[(i + 1) % permutation.size()];

        // Process node1
        for (auto del:this->f_delete[node1]) {
            del_mat(del.first, del.second) = true;
            del_mat(del.second, del.first) = true;
        }

        // Update fitness and validity
        if (!del_mat(node1, node2)) { // Assuming, that direct edge node1->node2 is the shortest path
            fitness += dist_mat(node1, node2);
        } else {
            vector<uint> path{};
            auto aStar_fitness = Astar(dist_mat, del_mat, node1, node2, path);
            fitness += aStar_fitness;

            for (uint j = 0; j < path.size() - 1; j++) {
                valid = valid && !del_mat(path[j], path[j + 1]);
            }
        }
    }

    fitness_evals++;
    return valid;
}

uint wTSPSDInstance::compute_dist(uint id1, uint id2) {
    if (id1 == id2) { return 0; }
    auto xdiff = this->positions[id1].x - this->positions[id2].x;
    auto ydiff = this->positions[id1].y - this->positions[id2].y;
    double distance = sqrt(xdiff*xdiff + ydiff*ydiff);
    return (uint)round(distance);
}

void wTSPSDInstance::compute_dist_mat() {
    for (uint i = 0; i < this->node_cnt; i++){
        for (uint j = 0; j <= i; j++){
            this->dist_mat(i, j) = this->compute_dist(i, j);
            this->dist_mat(j, i) = this->dist_mat(i, j);
        }
    }
}

uint wTSPSDInstance::closest_unused(const vector<fitness_t>& dist, const vector<bool>& sptSet) {
    fitness_t min = std::numeric_limits<fitness_t>::max()/2;
    uint min_index = std::numeric_limits<uint>::max()/2;

    for (uint v = 0; v < dist.size(); v++){
        if (!sptSet[v] && dist[v] <= min) {
            min = dist[v], min_index = v;
        }
    }

    return min_index;
}

fitness_t wTSPSDInstance::dijkstra(const boost::numeric::ublas::matrix<fitness_t>& graph, uint start, uint goal, vector<uint> &path) {
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

void reconstructPath(uint start, uint goal, vector<uint> &path, vector<uint> &prev) {
    auto current = goal;
    do {
        path.push_back(current);
        current = prev[current];
    } while (current != start);
    path.push_back(current);
    reverse(path.begin(), path.end());
}

fitness_t wTSPSDInstance::Astar(const boost::numeric::ublas::matrix<fitness_t>& dist_mat, const boost::numeric::ublas::matrix<bool> &del_mat, uint start, uint goal, vector<uint> &path) {
    typedef std::pair<fitness_t , uint> my_pair;
    auto size = dist_mat.size1();

    std::priority_queue<my_pair, vector<my_pair>, std::greater<my_pair>> openSet;               // priority queue <fScore, node>
    std::vector<uint> cameFrom(size);                                                        // immediate predecessors
    std::vector<fitness_t> gScore(size, std::numeric_limits<fitness_t>::max()/2);        // costs of cheapest paths
    std::vector<fitness_t> fScore(size, std::numeric_limits<fitness_t>::max()/2);        // cost estimates of cheapest paths to goal

    gScore[start] = 0;
    fScore[start] = dist_mat(start, goal); // h value
    openSet.push(std::make_pair(fScore[start], start));

    while (!openSet.empty()) {
        my_pair current = openSet.top();
        if (current.first != fScore[current.second]) { // fScore is not up-to-date
            openSet.pop();
        } else {
            if (current.second == goal) {
                reconstructPath(start, goal, path, cameFrom);
                return gScore[current.second];
            }
            // Pop current and expand it
            openSet.pop();
            for (uint neighbor = 0; neighbor < size; neighbor++) {
                if (neighbor != current.second) {
                    auto tentative_gScore = gScore[current.second] + del_mat(current.second, neighbor) * DELETED_EDGE_PENALTY + (1 - del_mat(current.second, neighbor)) * dist_mat(current.second, neighbor);
                    if (tentative_gScore < gScore[neighbor]) {
                        cameFrom[neighbor] = current.second;
                        gScore[neighbor] = tentative_gScore;
                        fScore[neighbor] = tentative_gScore + dist_mat(neighbor, goal); // h value
                        openSet.push(std::make_pair(fScore[neighbor], neighbor));
                    }
                }
            }
        }

    }

    std::cout << "A*: path not found" << std::endl;
    return std::numeric_limits<fitness_t>::max()/2;
}

void wTSPSDInstance::export_walk_orig_ids(vector<uint> &permutation, json &container) {
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






