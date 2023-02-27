#include "CVRP.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

using std::vector;
using permutator::fitness_t;

// ----------------------------------------------------------------------
// CVRPInstance
// ----------------------------------------------------------------------

CVRPInstance::CVRPInstance(const char* path, uint count, uint tours)
        : Instance(generateName(path, "CVRP"), count, 1, 1) {
    this->quantities = new uint[count];
    this->positions = new coords[count];
    this->tours = tours;
    this->dist_mat.resize(count, count);
    this->ubs[this->depot_id] = UINT_MAX;
    this->penalty_func_cnt = 4;
    this->parseDataFrom(path);
    this->compute_dist_mat();
}

CVRPInstance::~CVRPInstance(){
    delete[] this->quantities;
    delete[] this->positions;
}

// the authors of dataset suggest rounding the distances
// to the closest integer value, following the TSPLIB standard.
uint CVRPInstance::compute_dist(uint id1, uint id2) const {
    if (id1 == id2) { return 0; }
    uint xdiff = this->positions[id1].x - this->positions[id2].x;
    uint ydiff = this->positions[id1].y - this->positions[id2].y;
    double distance = sqrt(xdiff*xdiff + ydiff*ydiff);
    return (uint)round(distance);
}

void CVRPInstance::compute_dist_mat(){
    for (uint i = 0; i < this->node_cnt; i++){
        for (uint j = 0; j <= i; j++){
            this->dist_mat(i, j) = this->compute_dist(i, j);
            this->dist_mat(j, i) = this->dist_mat(i, j);
        }
    }
}

void CVRPInstance::processXMLNode(TiXmlNode* pParent, uint id){
	if ( !pParent ) return;
	TiXmlNode* pChild;
    TiXmlElement* pElem;
	int t = pParent->Type();
	if (t == TiXmlNode::TINYXML_ELEMENT) {
        pElem = pParent->ToElement();
        if (strcmp(pElem->Value(), "node") == 0) {
            pElem->QueryUnsignedAttribute("id", &id);
        } else if (strcmp(pElem->Value(), "cx") == 0) {
            this->positions[id-1].x = atoi(pElem->GetText());
        } else if (strcmp(pElem->Value(), "cy") == 0) {
            this->positions[id-1].y = atoi(pElem->GetText());
        } else if (strcmp(pElem->Value(), "request") == 0){
            pElem->QueryUnsignedAttribute("node", &id);
        } else if (strcmp(pElem->Value(), "quantity") == 0){
            this->quantities[id-1] = atoi(pElem->GetText());
            this->total_requests += this->quantities[id-1];
        } else if (strcmp(pElem->Value(), "capacity") == 0){
            this->car_capacity = strtoul(pElem->GetText(), nullptr, 10);
        } else if (strcmp(pElem->Value(), "departure_node") == 0){
            this->depot_id = strtoul(pElem->GetText(), nullptr, 10) - 1;
        } 
	}
	for ( pChild = pParent->FirstChild(); pChild != nullptr; pChild = pChild->NextSibling()) {
		processXMLNode(pChild, id);
	}
}

void CVRPInstance::readXMLFile(TiXmlDocument* doc) {
    processXMLNode(doc, -1);
}

void CVRPInstance::parseDataFrom(const char* path) {
    TiXmlDocument doc(path);
    bool loadOkay = doc.LoadFile();
	if (loadOkay){
		printf("Reading '%s':\n", path);
		readXMLFile( &doc );
	} else {
		printf("Failed to load file \"%s\"\n", path);
        throw std::system_error(ENOENT, std::system_category(), path);
	}
}

bool CVRPInstance::computeFitness(const vector<uint> &permutation, fitness_t &fitness, vector<fitness_t> &penalties) {
    uint curr_tour = 0, idx = 0;
    uint cargo_load = this->car_capacity;
    uint unsatisfied = this->total_requests;
    bool* visited = new bool[this->node_cnt] {false};
    bool is_feasible = true;
    uint prev_id = permutation[idx++];
    visited[this->depot_id] = true;
    fitness = 0;
    penalties.clear();

    for (; idx < permutation.size(); idx++) {
        const uint node_id = permutation[idx];
        fitness += this->dist_mat(prev_id, node_id);
        if (prev_id == this->depot_id) curr_tour++;
        if (node_id == this->depot_id) {
            cargo_load = this->car_capacity; // Fill car with cargo
        } else {
            if (!visited[node_id]) {
                if (cargo_load  >= this->quantities[node_id]) {
                    cargo_load -= this->quantities[node_id]; // Unload car
                    unsatisfied -= this->quantities[node_id];
                    visited[node_id] = true;
                }
            }
        }
        prev_id = node_id;
    }

    penalties.push_back(fitness);

    if (unsatisfied > 0) {
        fitness += 1000*unsatisfied;
        fitness += JOB_MISSING_PENALTY;
        is_feasible = false;
    }
    penalties.push_back(unsatisfied);

    if (curr_tour != this->tours){
        fitness += (JOB_MISSING_PENALTY / 10) * (abs((int)curr_tour - (int)this->tours));
        is_feasible = false;
    }
    penalties.push_back(abs((int)curr_tour - (int)this->tours));


    if (permutation.back() != this->depot_id || permutation.front() != this->depot_id) {
        fitness += JOB_MISSING_PENALTY;
        is_feasible = false;
    }
    penalties.push_back((uint)(permutation.back() != this->depot_id) + (uint)(permutation.front() != this->depot_id));

    delete[] visited;
    return is_feasible;
}

void CVRPInstance::print_nodes() {
    printf("Car capacity = %u, Number of tours = %u\n", this->car_capacity, this->tours);
    for (uint i = 0; i < this->node_cnt; i++) {
        printf("[%d]: (%d, %d) lb=%u, ub=%u, quant=%u\n",
            i, this->positions[i].x, this->positions[i].y,
            this->lbs[i], this->ubs[i], this->quantities[i]);
    }
}

void CVRPInstance::print_solution(Solution *solution, std::basic_ostream<char>& outf) {
    uint curr_tour = 0, idx = 0, cost = 0;
    bool* visited = new bool[this->node_cnt] {false};
    uint cargo_load = this->car_capacity;
    uint unsatisfied = this->total_requests;
    uint prev_id = solution->permutation[idx++];
    visited[this->depot_id] = true;

    outf << this->depot_id << "(" << this->quantities[this->depot_id] <<"/" << cargo_load << ")";
    for (; idx < solution->permutation.size(); idx++) {
        const uint node_id = solution->permutation[idx];
        if (node_id == this->depot_id) outf << std::endl;
        if (prev_id == this->depot_id) curr_tour++;
        if (node_id == this->depot_id) {
            cargo_load = this->car_capacity; // Fill car with cargo
        } else {
            if (!visited[node_id]) {
                if (cargo_load >= this->quantities[node_id]) {
                    cargo_load -= this->quantities[node_id]; // Unload car
                    unsatisfied -= this->quantities[node_id];
                    visited[node_id] = true;
                }
            }
        }
        cost += this->dist_mat(prev_id, node_id);
        outf  << " -(" << cost << ")> " << node_id << "(" << this->quantities[node_id] <<"/" << cargo_load << ")";
        prev_id = node_id;
    }
    outf << std::endl;
    outf << "Fitness: " << solution->fitness << "\nFeasibility: " << solution->is_feasible <<
         "\nTours: " << curr_tour << "\nUnsatisfied: " << unsatisfied << std::endl;
    outf << "Unvisited: {";
    for (uint i = 0; i < this->node_cnt; i++) {
        if (!visited[i]) {
            outf << i << ",";
        }
    }
    outf << "}" << std::endl;
    delete[] visited;
}