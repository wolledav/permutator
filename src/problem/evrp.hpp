#pragma once

#include "generic/instance.hpp"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>

#define CAPACITY_PENALTY 100000000
#define ENERGY_PENALTY 100000000

using namespace std;

struct Point{
    float x, y;
    float distance(const Point &p) const {
        return sqrt((this->x - p.x)*(this->x - p.x) + (this->x - p.y)*(this->x - p.y)); }
};


class EVRPInstance: public Instance
{
    private:
        float capacity; // load capacity
        float energy_cap; // energy capacity
        float energy_cons; // energy consumption

        uint route_cnt;
        uint customer_cnt;
        uint station_cnt;

        uint depot_id;
        vector<float> demands;
        vector<bool> station;

        vector<Point> coords;
        boost::numeric::ublas::matrix<float> dist_mat;

        void parse_json(const char* path);
        void calculate_dist_matrix();
        void calculate_lbs_ubs();

        uint get_int_id(const string& str) { return stoi(str) - 1; }
        string get_orig_id(const uint id) { return to_string(id + 1); }


    public:
        string type = "evrp";

        EVRPInstance(const char* path);
        ~EVRPInstance();

        bool compute_fitness(const vector<uint> &permutation, fitness_t* fitness) override;
};
