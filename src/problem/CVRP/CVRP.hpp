#pragma once

#include "generic/instance.hpp"
#include "lib/tinyxml/tinyxml.h"
#include "generic/solution.hpp"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <climits>

#define JOB_MISSING_PENALTY 100000000

class CVRPInstance: public Instance
{
    private:
        uint total_requests = 0;
        void readXMLFile(TiXmlDocument* doc);
        void processXMLNode(TiXmlNode* pParent, uint id);
        uint compute_dist(uint id1, uint id2) const;
        void compute_dist_mat();
    public:
        std::string type = "CVRP";
        uint tours;
        uint depot_id = 0;
        uint car_capacity;
        uint* quantities;
        coords* positions;
        boost::numeric::ublas::matrix<uint> dist_mat;

        CVRPInstance(const char* path, uint count, uint tours);
        ~CVRPInstance();
        bool computeFitness(const std::vector<uint> &permutation, permutator::fitness_t* fitness) override;
        void parseDataFrom(const char* path);
        void print_nodes();
        void print_solution(Solution *solution, std::basic_ostream<char>& outf = std::cout);
};
