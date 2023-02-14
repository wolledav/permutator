#include "EVRP.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

using permutator::fitness_t;
using std::vector;

// ----------------------------------------------------------------------
// EVRPInstance
// ----------------------------------------------------------------------

EVRPInstance::EVRPInstance(const char *path)
    : Instance(generateName(path, "EVRP"), 1, 1, 1)
{
    // this->quantities = new uint[count];
    // this->positions = new coords[count];
    // this->tours = tours;
    // this->dist_mat.resize(count, count);
    // this->ubs[this->depot_id] = UINT_MAX;
    this->parseDataFrom(path);
    this->compute_dist_mat();
}

EVRPInstance::~EVRPInstance()
{
    delete[] this->quantities;
    delete[] this->positions;
}

// the authors of dataset suggest rounding the distances
// to the closest integer value, following the TSPLIB standard.
uint EVRPInstance::compute_dist(uint id1, uint id2) const
{
    if (id1 == id2)
    {
        return 0;
    }
    uint xdiff = this->positions[id1].x - this->positions[id2].x;
    uint ydiff = this->positions[id1].y - this->positions[id2].y;
    double distance = sqrt(xdiff * xdiff + ydiff * ydiff);
    return (uint)round(distance);
}

void EVRPInstance::compute_dist_mat()
{
    for (uint i = 0; i < this->node_cnt; i++)
    {
        for (uint j = 0; j <= i; j++)
        {
            this->dist_mat(i, j) = this->compute_dist(i, j)*this->energy_consumption;
            this->dist_mat(j, i) = this->dist_mat(i, j);
        }
    }
}

void EVRPInstance::parseDataFrom(const char *path)
{
    int i;
    int problem_size;
    int lineLength = 100;
    char line[lineLength];
    char *keywords;
    char Delimiters[] = " :=\n\t\r\f\v";
    std::ifstream fin(path);
    while ((fin.getline(line, lineLength - 1)))
    {

        if (!(keywords = strtok(line, Delimiters)))
            continue;
        if (!strcmp(keywords, "DIMENSION"))
        {
            if (!sscanf(strtok(NULL, Delimiters), "%d", &this->num_customers))
            {
                std::cout << "DIMENSION error" << std::endl;
                exit(0);
            }
            this->num_customers -= 1; // depot included in DIMENSION
        }
        else if (!strcmp(keywords, "EDGE_WEIGHT_TYPE"))
        {
            char *tempChar;
            if (!(tempChar = strtok(NULL, Delimiters)))
            {
                std::cout << "EDGE_WEIGHT_TYPE error" << std::endl;
                exit(0);
            }
            if (strcmp(tempChar, "EUC_2D"))
            {
                std::cout << "not EUC_2D" << std::endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "CAPACITY"))
        {
            if (!sscanf(strtok(NULL, Delimiters), "%d", &this->car_capacity))
            {
                std::cout << "CAPACITY error" << std::endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "VEHICLES"))
        {
            if (!sscanf(strtok(NULL, Delimiters), "%d", &this->tours))
            {
                std::cout << "VEHICLES error" << std::endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "ENERGY_CAPACITY"))
        {
            if (!sscanf(strtok(NULL, Delimiters), "%d", &this->battery_capacity))
            {
                std::cout << "ENERGY_CAPACITY error" << std::endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "ENERGY_CONSUMPTION"))
        {
            if (!sscanf(strtok(NULL, Delimiters), "%lf", &this->energy_consumption))
            {
                std::cout << "ENERGY_CONSUMPTION error" << std::endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "STATIONS"))
        {
            if (!sscanf(strtok(NULL, Delimiters), "%d", &this->num_chargers))
            {
                std::cout << "STATIONS error" << std::endl;
                exit(0);
            }
        }
        // else if (!strcmp(keywords, "OPTIMAL_VALUE"))
        // {
        //     if (!sscanf(strtok(NULL, Delimiters), "%lf", &OPTIMUM))
        //     {
        //         std::cout << "OPTIMAL_VALUE error" << std::endl;
        //         exit(0);
        //     }
        // }
        else if (!strcmp(keywords, "NODE_COORD_SECTION"))
        {
            this->num_nodes = this->num_chargers + this->num_customers + 1;
            if (this->num_nodes != 0)
            {
                uint temp;
                this->positions = new coords[this->num_nodes];

                for (i = 0; i < this->num_nodes; i++)
                {
                    fin >> temp;
                    fin >> this->positions[i].x >> this->positions[i].y;
                }

                this->compute_dist_mat();
            }
            else
            {
                std::cout << "wrong problem instance file" << std::endl;
                exit(1);
            }
        }
        else if (!strcmp(keywords, "DEMAND_SECTION"))
        {
            if (this->num_nodes != 0)
            {

                int temp;
                this->quantities = new uint[this->num_customers];
                this->chargers = new bool[this->num_chargers] {true};

                for (i = 0; i < this->num_customers; i++)
                {
                    fin >> temp;
                    fin >> this->quantities[temp - 1];
                    this->chargers[temp - 1] = false;  //customer can't be a charging station
                }
            }
        }
        else if (!strcmp(keywords, "DEPOT_SECTION"))
        {
            fin >> this->depot_id;
            this->depot_id -= 1;
        }
    }
    fin.close();
    if (this->num_nodes == 0)
    {
        std::cout << "wrong problem instance file" << std::endl;
        exit(1);
    }
}

bool EVRPInstance::computeFitness(const vector<uint> &permutation, fitness_t *fitness)
{
    uint curr_tour = 0, idx = 0;
    uint cargo_load = this->car_capacity;
    uint unsatisfied = this->total_requests;
    bool *visited = new bool[this->node_cnt]{false};
    bool is_feasible = true;
    uint prev_id = permutation[idx++];
    visited[this->depot_id] = true;
    *fitness = 0;
    for (; idx < permutation.size(); idx++)
    {
        const uint node_id = permutation[idx];
        *fitness += this->dist_mat(prev_id, node_id);
        if (prev_id == this->depot_id)
            curr_tour++;
        if (node_id == this->depot_id)
        {
            cargo_load = this->car_capacity; // Fill car with cargo
        }
        else
        {
            if (!visited[node_id])
            {
                if (cargo_load >= this->quantities[node_id])
                {
                    cargo_load -= this->quantities[node_id]; // Unload car
                    unsatisfied -= this->quantities[node_id];
                    visited[node_id] = true;
                }
            }
        }
        prev_id = node_id;
    }
    if (unsatisfied > 0)
    {
        *fitness += 1000 * unsatisfied;
        *fitness += JOB_MISSING_PENALTY;
        is_feasible = false;
    }
    if (curr_tour != this->tours)
    {
        *fitness += (JOB_MISSING_PENALTY / 10) * (abs((int)curr_tour - (int)this->tours));
        is_feasible = false;
    }
    if (permutation.back() != this->depot_id || permutation.front() != this->depot_id)
    {
        *fitness += JOB_MISSING_PENALTY;
        is_feasible = false;
    }
    delete[] visited;
    return is_feasible;
}

void EVRPInstance::print_nodes()
{
    printf("Car capacity = %u, Number of tours = %u\n", this->car_capacity, this->tours);
    for (uint i = 0; i < this->node_cnt; i++)
    {
        printf("[%d]: (%d, %d) lb=%u, ub=%u, quant=%u\n",
               i, this->positions[i].x, this->positions[i].y,
               this->lbs[i], this->ubs[i], this->quantities[i]);
    }
}

void EVRPInstance::print_solution(Solution *solution, std::basic_ostream<char> &outf)
{
    uint curr_tour = 0, idx = 0, cost = 0;
    bool *visited = new bool[this->node_cnt]{false};
    uint cargo_load = this->car_capacity;
    uint unsatisfied = this->total_requests;
    uint prev_id = solution->permutation[idx++];
    visited[this->depot_id] = true;

    outf << this->depot_id << "(" << this->quantities[this->depot_id] << "/" << cargo_load << ")";
    for (; idx < solution->permutation.size(); idx++)
    {
        const uint node_id = solution->permutation[idx];
        if (node_id == this->depot_id)
            outf << std::endl;
        if (prev_id == this->depot_id)
            curr_tour++;
        if (node_id == this->depot_id)
        {
            cargo_load = this->car_capacity; // Fill car with cargo
        }
        else
        {
            if (!visited[node_id])
            {
                if (cargo_load >= this->quantities[node_id])
                {
                    cargo_load -= this->quantities[node_id]; // Unload car
                    unsatisfied -= this->quantities[node_id];
                    visited[node_id] = true;
                }
            }
        }
        cost += this->dist_mat(prev_id, node_id);
        outf << " -(" << cost << ")> " << node_id << "(" << this->quantities[node_id] << "/" << cargo_load << ")";
        prev_id = node_id;
    }
    outf << std::endl;
    outf << "Fitness: " << solution->fitness << "\nFeasibility: " << solution->is_feasible << "\nTours: " << curr_tour << "\nUnsatisfied: " << unsatisfied << std::endl;
    outf << "Unvisited: {";
    for (uint i = 0; i < this->node_cnt; i++)
    {
        if (!visited[i])
        {
            outf << i << ",";
        }
    }
    outf << "}" << std::endl;
    delete[] visited;
}