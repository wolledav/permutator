#include "evrp.hpp"

#include <fstream>
#include <algorithm>

#include "lib/nlohmann/json.hpp"
// ----------------------------------------------------------------------
// EVRPInstance
// TODO: whole problem
// ----------------------------------------------------------------------

using namespace std;

EVRPInstance::EVRPInstance(const char* path) : Instance()
{
    this->parse_json(path);
    this->calculate_dist_matrix();
    this->calculate_lbs_ubs();
}

EVRPInstance::~EVRPInstance(){
    delete[] this->demands;
    delete[] this->station;
}

void EVRPInstance::parse_json(const char* path)
{
    json data = read_json(path);
    
    // header
    this->name = data["name"];

    this->capacity = data["capacity"];
    this->energy_cap = data["energy_capacity"];
    this->energy_cons = data["energy_consumption"];

    this->route_cnt = data["vehicles"];
    this->customer_cnt = data["dimension"];
    this->station_cnt = data["stations"];
    this->node_cnt = this->customer_cnt + this->station_cnt;

    // coord section
    this->coords.resize(this->node_cnt);
    for (const auto& item: data["node_coord"].items()) {
        uint id = this->get_int_id(item.key());
        this->coords[id].x = item.value()[0];
        this->coords[id].y = item.value()[1];
    }
    // demand section
    this->total_demand = 0;
    this->demands = new float[this->node_cnt] {0};
    for (const auto& item: data["node_demand"].items()) {
        uint id = this->get_int_id(item.key());
        this->demands[id] = item.value();
        this->total_demand += this->demands[id];
    }

    // station section
    this->station = new bool[this->node_cnt] {false};
    for (const auto& item: data["stations_id"]) {
        uint id = this->get_int_id(item);
        this->station[id] = true;
    }

    this->depot_id = this->get_int_id(data["depot"]);

    // additional stats
    
}

void EVRPInstance::calculate_dist_matrix()
{
    float d;
    this->dist_mat.resize(this->node_cnt, this->node_cnt);
    for (uint i = 0; i < this->node_cnt; i++) {
        this->dist_mat(i, i) = 0;
        for (uint j = i+1; j < this->node_cnt; j++) {
            d = this->coords[i].distance(this->coords[j]);
            this->dist_mat(i, j) = d;
            this->dist_mat(j, i) = d;
        }
    }
}

void EVRPInstance::calculate_lbs_ubs()
{
    this->lbs.resize(this->node_cnt);
    this->ubs.resize(this->node_cnt);
    for (uint i = 0; i < this->node_cnt; i++) {
        if (i == this->depot_id) { // depot
            this->lbs[i] = this->route_cnt - 1; // depot final destination is implicit
            this->ubs[i] = this->customer_cnt - 1; // at worst we go (cust_1, dep, cust_2, ... dep, cust_n) 
        }
        else if (this->station[i]) { // station
            this->lbs[i] = 0; // at best we dont have to recharge
            this->ubs[i] = 2*this->customer_cnt; // at worst we recharge back and forth to each cust 
        }
        else { // customer
            this->lbs[i] = 1;
            this->ubs[i] = 1;
        }
    }
}

bool EVRPInstance::compute_fitness(const vector<uint> &permutation, fitness_t* fitness)
{
    float fit = 0; // original fitness function
    bool is_feasable = true;

    uint prev_node = this->depot_id; // we are implicitly starting at a depot
    float curr_load = this->capacity; // we start full capacity
    float curr_energy = this->energy_cap; // we start full energy
    float dist;

    bool* visited = new bool[this->node_cnt] {false};
    float unsatisfied = this->total_demand;
    float missing_energy = 0;
    uint no_move_viol = 0;


    for (const auto node: permutation) {
        dist = this->dist_mat(prev_node, node);
        fit += dist;

        curr_energy -= dist*this->energy_cons;
        if (curr_energy < 0) {
            missing_energy -= curr_energy;
        }


        if (node == prev_node) {
            no_move_viol += 1;
        }
        
        else if (node == this->depot_id) {
            curr_load = this->capacity;
            curr_energy = this->energy_cap;
        }
        else if (this->station[node]) {
            curr_energy = this->energy_cap;
        }

        else if (!visited[node] && (curr_load >= this->demands[node])) {
            curr_load -= this->demands[node];
            unsatisfied -= this->demands[node];
            visited[node] = true;
        }
    }

    delete[] visited;

    // we implicitly finishing at a depot
    dist = this->dist_mat(prev_node, this->depot_id); 
    fit += dist;
    
    curr_energy -= dist*this->energy_cons;
    if (curr_energy < 0) {
        missing_energy -= curr_energy;
    }

    if (prev_node == this->depot_id) {
        no_move_viol += 1;
    }

    if (unsatisfied > 0) {
        fit += CAPACITY_PENALTY*(1 + unsatisfied/1000);
        is_feasable = false;
    }
    if (missing_energy > 0) {
        fit += ENERGY_PENALTY*(1 + missing_energy/10000);
        is_feasable = false;
    }
    if (no_move_viol > 0) {
        fit += NO_MOVE_PENALTY*no_move_viol;
        is_feasable = false;
    }

    *fitness = (fitness_t)round(fit);
    return is_feasable;
}


void EVRPInstance::print_solution(Solution *solution, std::basic_ostream<char>& outf)
{
    outf << "ERVP solutiun, printing tours in <node id>(<load>/<energy>)" << endl;

    uint cap_viol = 0; // number of node with violating capacity
    uint energy_viol = 0; // number of node with violating energy

    uint prev_node = this->depot_id; // we are implicitly starting at a depot
    float curr_cap = this->capacity; // we start full capacity
    float curr_energy = this->energy_cap; // we start full energy
    float dist;

    outf << "    " << this->get_orig_id(this->depot_id) << "[D](" << curr_cap << "/" << curr_energy << ")";

    uint tours = 1;

    for (const auto node: solution->permutation) {
        dist = this->dist_mat(prev_node, node);

        curr_cap -= this->demands[node];
        curr_energy -= dist*this->energy_cons;

        if ((node == this->depot_id) && (prev_node == this->depot_id)) continue;

        if (node == this->depot_id) {
            outf << " -|" << (int)round(dist) << "|> " << this->get_orig_id(node) << "[D](" << curr_cap << "/" << (int)round(curr_energy) << ")" << endl;
            curr_cap = this->capacity;
            curr_energy = this->energy_cap;
            outf << "    " << this->get_orig_id(node) << "[D](" << curr_cap << "/" << (int)round(curr_energy) << ")";
            tours += 1;
        }

        else if (this->station[node]) {
            outf << " -|" << (int)round(dist) << "|> " << this->get_orig_id(node) << "[S](" << curr_cap << "/" << (int)round(curr_energy) << " R " << this->energy_cap << ")";
            curr_energy = this->energy_cap;
        }

        else {
            outf << " -|" << (int)round(dist) << "|> " << this->get_orig_id(node) << "(" << curr_cap << "/" << (int)round(curr_energy) << ")";
        }

        curr_cap = (node == this->depot_id) 
            ? this->capacity : curr_cap; // refill capacity in depot

        curr_energy = ((node == this->depot_id) || this->station[node])
            ? this->energy_cap : curr_energy; // refill energy in depot or station

        prev_node = node;
    }
    dist = this->dist_mat(prev_node, this->depot_id); // we are finishing starting at a depot
    curr_energy -= dist*this->energy_cons;

    outf << " -|" << (int)round(dist) << "|> " << this->get_orig_id(this->depot_id) << "[D](" << curr_cap << "/" << (int)round(curr_energy) << ")" << endl;

    outf << "Fitness: " << solution->fitness << endl;
    outf << "Feasibility: " << solution->is_feasible << endl;
    outf << "Tours " << tours << endl;
}