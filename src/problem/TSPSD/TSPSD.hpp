#pragma once

#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/functional/hash.hpp>

#include "generic/instance.hpp"
#include "lib/nlohmann/json.hpp"

#define DELETED_EDGE_PENALTY 1000000

class TSPSDInstance : public Instance {
private:
    uint compute_dist(uint id1, uint id2);
    void compute_dist_mat();

public:
    std::vector<coords> positions;
    boost::numeric::ublas::matrix<uint> dist_mat;
    std::vector<std::vector<std::pair<uint, uint>>> f_delete;
//    boost::numeric::ublas::symmetric_matrix<std::vector<bool>> deleted_by;

    static uint get_internal_id(const std::string& id);
    static std::string get_original_id(uint id);
    static void export_perm_orig_ids(std::vector<uint>& perm, nlohmann::json& container);

    explicit TSPSDInstance(const char* path);
    bool computeFitness(const std::vector<uint> &permutation, permutator::fitness_t &fitness, std::vector<permutator::fitness_t> &penalties) override;
};

