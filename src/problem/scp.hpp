//
// Created by wolledav on 24.8.22.
//

#pragma once

#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/functional/hash.hpp>

#include "generic/instance.hpp"
#include "lib/nlohmann/json.hpp"

#define DELETED_EDGE_PENALTY 1000000

using json = nlohmann::json;

class SCPInstance : public Instance {
private:
    uint compute_dist(uint id1, uint id2);
    void compute_dist_mat();

public:
    std::vector<coords> positions;
    boost::numeric::ublas::matrix<uint> dist_mat;
    std::vector<std::vector<std::pair<uint, uint>>> f_delete;
//    boost::numeric::ublas::symmetric_matrix<std::vector<bool>> deleted_by;

    static uint get_internal_id(const string& id);
    static string get_original_id(uint id);
    static void export_perm_orig_ids(vector<uint>& perm, json& container);

    explicit SCPInstance(const char* path);
    bool compute_fitness(const vector<uint> &permutation, fitness_t* fitness) override;

};

