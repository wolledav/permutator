//
// Created by wolledav on 24.8.22.
//

#pragma once

#include <iostream>
#include <limits.h>
#include <boost/numeric/ublas/matrix.hpp>

#include "generic/instance.hpp"
#include "lib/nlohmann/json.hpp"

#define DELETED_EDGE_PENALTY 1000000

using json = nlohmann::json;

class WCPInstance : public Instance {
private:
    static uint closest_unused(const vector<fitness_t>& dist, const vector<bool>& sptSet);
    static fitness_t dijkstra(const boost::numeric::ublas::matrix<fitness_t>& graph, uint start, uint goal, vector<uint> &path);
    uint compute_dist(uint id1, uint id2);
    void compute_dist_mat();

public:
    std::vector<coords> positions;
    boost::numeric::ublas::matrix<uint> dist_mat;
    std::vector<std::vector<uint>> f_delete;

    static uint get_internal_id(const string& id);
    static string get_original_id(uint id);
    static void export_perm_orig_ids(vector<uint>& perm, json& container);

    explicit WCPInstance(const char* path);
    bool compute_fitness(const vector<uint> &permutation, fitness_t* fitness) override;
    void export_walk_orig_ids(vector<uint>& permutation, json& container);

};

