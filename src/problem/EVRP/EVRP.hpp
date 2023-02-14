#pragma once

#include "generic/instance.hpp"
#include "lib/tinyxml/tinyxml.h"
#include "generic/solution.hpp"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <climits>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<cstring>
#include<math.h>
#include<fstream>
#include<time.h>
#include<limits.h>

#define JOB_MISSING_PENALTY 100000000



class EVRPInstance : public Instance
{
private:
    uint total_requests = 0;
    uint compute_dist(uint id1, uint id2) const;
    void compute_dist_mat();

public:
    std::string type = "EVRP";
    uint tours;
    uint depot_id = 0;
    uint num_nodes;
    uint num_chargers;
    uint num_customers;
    uint car_capacity;
    uint battery_capacity;
    double energy_consumption;
    uint *quantities;
    coords *positions;
    uint* customer_ids;
    bool* chargers;
    boost::numeric::ublas::matrix<uint> dist_mat;

    EVRPInstance(const char *path);
    ~EVRPInstance();
    bool computeFitness(const std::vector<uint> &permutation, permutator::fitness_t *fitness) override;
    void parseDataFrom(const char *path);
    void print_nodes();
    void print_solution(Solution *solution, std::basic_ostream<char> &outf = std::cout);
};
