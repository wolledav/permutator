#pragma once

#include "utils.hpp"
#include "generic/instance.hpp"
#include "lib/tinyxml/tinyxml.h"
#include "generic/solution.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/functional.hpp>

#define JOB_MISSING_PENALTY 100000000
#define NEGATIVE_CHARGE_PENALTY 100000000



class EVRPInstance : public Instance
{
private:
    uint total_requests = 0;
    double compute_dist(uint id1, uint id2) const;
    void compute_dist_mat();

public:
    std::string type = "EVRP";
    uint tours;
    uint depot_id = 0;
    uint num_chargers;
    uint num_customers;
    uint car_capacity;
    double battery_capacity;
    double energy_consumption;
    uint *quantities;
    coords *positions;
    bool *chargers;
    boost::numeric::ublas::matrix<double> dist_mat;


    EVRPInstance(const char *path, int cnt, int charger_cnt);
    ~EVRPInstance();
    bool computeFitness(const std::vector<uint> &permutation, permutator::fitness_t &fitness, std::vector<permutator::fitness_t> &penalties) override;
    void parseDataFrom(const char *path);
    void print_nodes();
    void print_solution(Solution *solution, std::basic_ostream<char> &outf = std::cout);
};
