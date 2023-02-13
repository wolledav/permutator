//
// Created by wolledav on 24.8.22.
//

#pragma once

#include <iostream>
#include <climits>
#include <boost/numeric/ublas/matrix.hpp>
#include <bits/stdc++.h>
#include <omp.h>

#include "generic/instance.hpp"
#include "lib/nlohmann/json.hpp"

#define DELETED_EDGE_PENALTY 1000000

class wTSPSDInstance : public Instance {
private:
    static uint closest_unused(const std::vector<permutator::fitness_t>& dist, const std::vector<bool>& sptSet);
    static permutator::fitness_t dijkstra(const boost::numeric::ublas::matrix<permutator::fitness_t>& graph, uint start, uint goal, std::vector<uint> &path);
    static permutator::fitness_t Astar(const boost::numeric::ublas::matrix<permutator::fitness_t>& dist_mat, const boost::numeric::ublas::matrix<bool> &del_mat, uint start, uint goal, std::vector<uint> &path);
    uint compute_dist(uint id1, uint id2);
    void compute_dist_mat();

public:
    std::vector<coords> positions;
    boost::numeric::ublas::matrix<uint, boost::numeric::ublas::row_major> dist_mat;
    std::vector<std::vector<std::pair<uint, uint>>> f_delete;

    static uint get_internal_id(const std::string& id);
    static std::string get_original_id(uint id);
    static void export_perm_orig_ids(std::vector<uint>& perm, nlohmann::json& container);

    explicit wTSPSDInstance(const char* path);
    bool computeFitness(const std::vector<uint> &permutation, permutator::fitness_t &fitness, std::vector<permutator::fitness_t> &penalties) override;
    void export_walk_orig_ids(std::vector<uint>& permutation, nlohmann::json& container);

};

