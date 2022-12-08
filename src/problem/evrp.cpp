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
    this->coords = vector<Point>(this->node_cnt);
    for (const auto& item: data["node_coord"].items()) {
        uint id = this->get_int_id(item.key());
        this->coords[id].x = item.value()[0];
        this->coords[id].y = item.value()[1];
    }
    // demand section
    this->total_demand = 0;
    this->demands = vector<float>(this->node_cnt);
    fill(this->demands.begin(), this->demands.end(), 0);
    for (const auto& item: data["node_demand"].items()) {
        uint id = this->get_int_id(item.key());
        this->demands[id] = item.value();
        this->total_demand += this->demands[id];
    }

    this->depot_id = this->get_int_id(data["depot"]);
    this->station_ids.push_back(this->depot_id);

    // station section
    this->station = vector<bool>(this->node_cnt);
    fill(this->station.begin(), this->station.end(), false);
    for (const auto& item: data["stations_id"]) {
        uint id = this->get_int_id(item);
        this->station[id] = true;
        this->station_ids.push_back(id);
    }

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
            this->lbs[i] = 2; // depot start and end
            this->ubs[i] = this->customer_cnt + 1; // at worst we go (dep, cust_1, dep, cust_2, ... dep, cust_n, dep) 
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

    float unsatisfied = this->total_demand;
    float missing_energy = 0;

    uint no_move = 0;

    uint prev_id, curr_id, perm_size = permutation.size();
    float dist, curr_load = this->capacity, curr_energy = this->energy_cap;

    for (uint i = 1; i < perm_size; i++) {
        prev_id = permutation[i-1];
        curr_id = permutation[i];

        if (curr_id == prev_id) {
            no_move += 1;
            continue;
        }
        
        dist = this->dist_mat(prev_id, curr_id);
        fit += dist;
        curr_energy -= dist*this->energy_cons;

        if (curr_energy < 0) {
            missing_energy -= curr_energy;
        }
        
        if (curr_id == this->depot_id) {
            curr_energy = this->energy_cap;
            curr_load = this->capacity;
        }
        else if (this->station[curr_id]) {
            curr_energy = this->energy_cap;
        }
        else {
            if (curr_load >= this->demands[curr_id]) {
                unsatisfied -= this->demands[curr_id];
            }
            curr_load -= this->demands[curr_id];
        }
    }

    if (missing_energy > 0) {
        fit += ENERGY_PENALTY*(1 + missing_energy/1000);
        is_feasable = false;
    }

    if (unsatisfied > 0) {
        fit += CAPACITY_PENALTY*(1 + unsatisfied/this->total_demand);
        is_feasable = false;
    }

    if (no_move > 0) {
        fit += 0.1*INVALID_SOL_PENALTY*(1 + no_move/this->customer_cnt);
        is_feasable = false;
    }

    if ((permutation.front() != this->depot_id) || (permutation.back() != this->depot_id)) {
        fit += INVALID_SOL_PENALTY;
        is_feasable = false;
    }

    *fitness = (fitness_t)round(fit);
    return is_feasable;
}


string EVRPInstance::node_to_str(uint id, float load, float energy)
{
    char buffer[256];
    snprintf(buffer, 256, "[%3s %s %5.0f/%5.0f]", 
        this->get_orig_id(id).c_str(), 
        ((this->depot_id == id) ? "D" : (this->station[id] ? "S" : " ")),
        load, energy);

    return string(buffer);
}

vector<uint> EVRPInstance::repair_func(const vector<uint> &permutation)
{
    vector<uint> tsp_route; // only customers
    for (auto x: permutation) {
        if (x != this->depot_id || this->station[x]) {
            tsp_route.push_back(x);
        }
    }

    return this->zga_repair(tsp_route);
}


vector<uint> EVRPInstance::zga_repair(const vector<uint> &tsp_tour)
{
    cout << "zga_repair" << endl;
    vector<uint> ervp_tour;
    uint next_id = 0;
    uint best_station, target;
    float load, charge, req_en, req_en_closest;
    bool found;
    
    ervp_tour.push_back(0);

    while (next_id < tsp_tour.size() || ervp_tour.back() != this->depot_id) {
        // try to get to next customer else get to depot
        if (next_id < tsp_tour.size()) {
            target = tsp_tour[next_id];
        }
        else {
            target = this->depot_id;
        }
        
        // if we run out of load we go to depot
        load = this->calc_load(ervp_tour).back();
        if (load < this->demands[target]) { // ran out of demands
            target = this->depot_id;
        }

        charge = this->calc_charge(ervp_tour).back();
        req_en = this->energy_req(ervp_tour.back(), target);
        if (target != this->depot_id) { 
            req_en_closest = this->energy_req(target, this->closest_station(target));
        }
        else {
            req_en_closest = 0;
        }
        
        if (charge >= req_en + req_en_closest) {
            ervp_tour.push_back(target);
            if (target != this->depot_id) {
                next_id++;
            }
        }
        else { // need to find station in reach, if none exists backtrack
            while (true) {
                charge = this->calc_charge(ervp_tour).back();
                found = this->closest_station_in_reach(best_station, target, ervp_tour.back(), charge);
                if (found) {
                    ervp_tour.push_back(best_station);
                    break;
                }
                else {
                    ervp_tour.pop_back();
                    next_id--;
                }
            }
        }
    }

    return ervp_tour;
}


bool EVRPInstance::closest_station_in_reach(uint &best_station, uint closest_to, uint in_reach, float charge)
{
    float best_dist = INFINITY, dist, req_en;
    bool found = false;

    for (auto &x : this->station_ids) {
        req_en = this->energy_req(x, in_reach);
        if (req_en <= charge && x != in_reach) {
            dist = this->dist_mat(x, closest_to);
            if (dist < best_dist) {
                best_dist = dist;
                best_station = x;
                found = true;
            }
        }
    }

    return found;
}

uint EVRPInstance::closest_station(uint closest_to)
{
    float best_dist = INFINITY, dist;
    uint best_station = 0;
    for (auto &x : this->station_ids) {
        if (x == closest_to) continue;

        dist = this->dist_mat(x, closest_to);
        if (dist < best_dist) {
            best_dist = dist;
            best_station = x;
        }
    }

    return best_station;
}

vector<float> EVRPInstance::calc_load(const vector<uint> &tour)
{
    float curr_load = this->capacity;
    vector<float> load(tour.size());
    for (auto &x : tour) {
        curr_load -= this->demands[x];
        load.push_back(curr_load);

        if (x == this->depot_id) {
            curr_load = this->capacity;
        }
    }

    return load;
}

vector<float> EVRPInstance::calc_charge(const vector<uint> &tour)
{
    float curr_charge = this->energy_cap;
    vector<float> charge(tour.size());
    uint from = this->depot_id, to;
    for(int i = 0; i + 1 < tour.size(); i++) {
        from = tour[i];
        to = tour[i + 1];
        curr_charge -= this->dist_mat(from, to)*this->energy_cons;
        charge.push_back(curr_charge);

        if (to == this->depot_id || this->station[to]) {
            curr_charge = this->energy_cap;
        }
    }

    charge.push_back(curr_charge);

    return charge;
}

float EVRPInstance::tour_dist(vector<uint> tour)
{
    float dist = 0;
    uint from, to;

    for (int i = 0; i + 1 < tour.size(); i++) {
        from = tour[i];
        to = tour[i + 1];
        dist += this->dist_mat(from, to);
    }

    return dist;
}


void EVRPInstance::print_solution(Solution *solution, std::basic_ostream<char>& outf)
{
    char print_buffer[256];
    float fit = 0; // original fitness function
    bool is_feasable = true;
    int routes = 0;

    auto permutation = solution->permutation;
    
    float unsatisfied = this->total_demand;
    float missing_energy = 0;

    uint no_move = 0;
    
    uint prev_id, curr_id, perm_size = permutation.size();
    float dist, curr_load = this->capacity, curr_energy = this->energy_cap;

    outf << "Route " << routes + 1 << " started" << endl << "    " 
        << this->node_to_str(permutation.front(), curr_load, curr_energy);

    for (uint i = 1; i < perm_size; i++) {
        prev_id = permutation[i-1];
        curr_id = permutation[i];

        dist = this->dist_mat(prev_id, curr_id);

        if (curr_id == prev_id) {
            no_move += 1;
            outf << " -x- ";
        }
        else {
            snprintf(print_buffer, 256, " --(%.2f)-> ", dist);
            outf << print_buffer;
        }
        
        fit += dist;
        curr_energy -= dist*this->energy_cons;

        if (curr_energy < 0) {
            missing_energy -= curr_energy;
        }
        outf << endl << "    " << this->node_to_str(curr_id, curr_load, curr_energy);
        
        if (curr_id == this->depot_id) {
            routes += 1;
            outf << endl << "Route " << routes << " ended" << endl << endl;
            curr_energy = this->energy_cap;
            curr_load = this->capacity;
            if (i + 1 < perm_size) {
            outf << "Route " << routes + 1 << " started" << endl << "    " 
                << this->node_to_str(curr_id, curr_load, curr_energy);
            }
        }
        else if (this->station[curr_id]) {
            curr_energy = this->energy_cap;
        }
        else {
            if (curr_load >= this->demands[curr_id]) {
                unsatisfied -= this->demands[curr_id];
            }
            curr_load -= this->demands[curr_id];
        }
    }

    if (missing_energy > 0) {
        fit += ENERGY_PENALTY*(1 + missing_energy/1000);
        is_feasable = false;
    }

    if (unsatisfied > 0) {
        fit += CAPACITY_PENALTY*(1 + unsatisfied/this->total_demand);
        is_feasable = false;
    }

    if (no_move > 0) {
        fit += 0.02*INVALID_SOL_PENALTY*(1+no_move/this->customer_cnt);
        is_feasable = false;
    }

    if ((permutation.front() != this->depot_id) || (permutation.back() != this->depot_id)) {
        fit += INVALID_SOL_PENALTY;
        is_feasable = false;
    }

    snprintf(print_buffer, 256, "Feasible: %s, Fitness %.0f, Unsatisfied: %.0f, Missing energy: %.0f, No move: %d",
        (is_feasable  ? "YES" : "NO"), fit, unsatisfied, missing_energy, no_move);

    outf << endl << print_buffer << endl;
}
