#pragma once

#include "generic/instance.hpp"
#include "generic/solution.hpp"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>

#define CAPACITY_PENALTY 50000
#define ENERGY_PENALTY 200000
#define INVALID_SOL_PENALTY 1000000

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
        float total_demand; // total sum of demands

        uint route_cnt;
        uint customer_cnt;
        uint station_cnt;
        uint total_cnt;

        uint depot_id;
        vector<float> demands;
        vector<bool> station;
        vector<uint> station_ids;

        vector<Point> coords;
        boost::numeric::ublas::matrix<float> dist_mat;

        void parse_json(const char* path);
        void calculate_dist_matrix();
        void calculate_lbs_ubs();

        uint get_int_id(const string& str) { return stoi(str) - 1; }
        string get_orig_id(const uint& id) { return to_string(id + 1); }

        vector<vector<uint>> split_tours(const vector<uint> perm);
        vector<float> calc_load(const vector<uint> &tour);
        vector<float> calc_charge(const vector<uint> &tour);
        float energy_req(uint from, uint to) { return this->dist_mat(from, to)*this->energy_cons; }
        float tour_dist(vector<uint> tour);

        vector<uint> zga_repair(const vector<uint> &permutation);

        void get_to_target(vector<uint> &ervp_tour, uint target);
        bool closest_station_in_reach(uint &best_station, uint closest_to, uint in_reach, float charge);
        uint closest_station (uint closest_to);

    public:
        string type = "evrp";

        EVRPInstance(const char* path);
        ~EVRPInstance();

        bool compute_fitness(const vector<uint> &permutation, fitness_t* fitness) override;
        vector<uint> repair_func(const vector<uint> &permutation);

        void print_solution(Solution *solution, std::basic_ostream<char>& outf = std::cout);
        string node_to_str(uint id, float load, float energy);
};
