#include "EVRP.hpp"

using permutator::fitness_t;
using std::vector;

// ----------------------------------------------------------------------
// EVRPInstance
// ----------------------------------------------------------------------

EVRPInstance::EVRPInstance(const char *path, int cnt, int charger_cnt)
    : Instance(generateName(path, "EVRP"), cnt + charger_cnt, 1, 1)
{
    this->parseDataFrom(path);
    this->ubs[this->depot_id] = this->tours+1;
    this->dist_mat.resize(this->node_cnt, this->node_cnt);
    this->penalty_func_cnt = 5;
    for (int i = 0; i < this->node_cnt; i++)
    {
        this->total_requests += this->quantities[i];
    }
    for (uint i = 0; i < this->node_cnt; i++)
    {
        if (this->chargers[i] && i != this->depot_id)
        {
            this->lbs[i] = 0;
            this->ubs[i] = 2 * cnt;
        }
    }
    this->compute_dist_mat();
}

EVRPInstance::~EVRPInstance()
{
    delete[] this->quantities;
    delete[] this->positions;
    delete[] this->chargers;
}

// the authors of dataset suggest rounding the distances
// to the closest integer value, following the TSPLIB standard.
double EVRPInstance::compute_dist(uint id1, uint id2) const
{
    if (id1 == id2)
    {
        return 0;
    }
    uint xdiff = this->positions[id1].x - this->positions[id2].x;
    uint ydiff = this->positions[id1].y - this->positions[id2].y;
    double distance = sqrt(xdiff * xdiff + ydiff * ydiff);
    return distance;
}

void EVRPInstance::compute_dist_mat()
{
    for (uint i = 0; i < this->node_cnt; i++)
    {
        for (uint j = 0; j <= i; j++)
        {
            this->dist_mat(i, j) = this->compute_dist(i, j);
            this->dist_mat(j, i) = this->dist_mat(i, j);
        }
    }
}

//part of implementation of EVRP.cpp provided by IEEE WCCI-2020 EVRP as part of the competition 
//web: https://mavrovouniotis.github.io/EVRPcompetition2020/
//code download link: https://mavrovouniotis.github.io/EVRPcompetition2020/SampleCode.zip
void EVRPInstance::parseDataFrom(const char *path)
{
    int i;
    int lineLength = 100;
    char *line = new char[lineLength];
    char *keywords;
    char Delimiters[] = " :=\n\t\r\f\v";
    std::ifstream fin(path);
    while (fin.getline(line, lineLength - 1))
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
            if (!sscanf(strtok(NULL, Delimiters), "%lf", &this->battery_capacity))
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
            this->node_cnt = this->num_chargers + this->num_customers + 1;
            if (this->node_cnt != 0)
            {
                uint temp;
                this->positions = new coords[this->node_cnt];

                for (i = 0; i < this->node_cnt; i++)
                {
                    fin >> temp;
                    fin >> this->positions[i].x >> this->positions[i].y;
                }
            }
            else
            {
                std::cout << "wrong problem instance file" << std::endl;
                exit(1);
            }
        }
        else if (!strcmp(keywords, "DEMAND_SECTION"))
        {
            if (this->node_cnt != 0)
            {

                int temp;
                this->quantities = new uint[this->node_cnt];

                for (i = 0; i < this->num_customers + 1; i++) // +1 for depot
                {
                    fin >> temp;
                    fin >> this->quantities[temp - 1];
                }
            }
        }
        else if (!strcmp(keywords, "STATIONS_COORD_SECTION"))
        {
            if (this->node_cnt != 0)
            {
                int idx;
                this->chargers = new bool[this->node_cnt]{false};

                for (i = 0; i < this->num_chargers; i++)
                {
                    fin >> idx;
                    this->quantities[idx - 1] = 0;
                    this->chargers[idx - 1] = true;
                }
            }
        }
        else if (!strcmp(keywords, "DEPOT_SECTION"))
        {
            fin >> this->depot_id;
            this->depot_id -= 1;
            this->chargers[this->depot_id] = true;
            this->quantities[this->depot_id] = 0;
        }
    }
    fin.close();
    if (this->node_cnt == 0)
    {
        std::cout << "wrong problem instance file" << std::endl;
        exit(1);
    }
}

bool EVRPInstance::computeFitness(const vector<uint> &permutation, fitness_t &fitness, vector<fitness_t> &penalties)
{
    uint curr_tour = 0, idx = 0;
    uint cargo_load = this->car_capacity;
    uint unsatisfied = this->total_requests;
    double charge = this->battery_capacity;
    double distance, negative_charge = 0;
    double temp_fitness = 0; //provided optima are floats, this is intended to minimize the rounding error
    bool *visited = new bool[this->node_cnt]{false};
    bool is_feasible = true;
    uint prev_id = permutation[idx++];
    visited[this->depot_id] = true;
    fitness = 0;
    penalties.clear();

    for (; idx < permutation.size(); idx++)
    {
        const uint node_id = permutation[idx];
        distance = this->dist_mat(prev_id, node_id);
        temp_fitness += distance;
        charge -= distance * this->energy_consumption;

        if (charge < 0)
        {
            negative_charge += distance * this->energy_consumption;
        }
        if (prev_id == this->depot_id)
            curr_tour++;
        if (node_id == this->depot_id)
        {
            cargo_load = this->car_capacity; // Fill car with cargo
            charge = this->battery_capacity;
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
        if (this->chargers[node_id])
        {
            charge = this->battery_capacity;
        }
        prev_id = node_id;
    }

    fitness = (fitness_t)round(temp_fitness);
    penalties.push_back(fitness);

    if (unsatisfied > 0)
    {
        fitness += 1000 * unsatisfied;
        fitness += JOB_MISSING_PENALTY;
        is_feasible = false;
    }
    penalties.push_back(unsatisfied);

    if (curr_tour != this->tours)
    {
        fitness += (JOB_MISSING_PENALTY / 10) * (abs((int)curr_tour - (int)this->tours));
        is_feasible = false;
    }
    penalties.push_back(std::max(0, abs((int)curr_tour - (int)this->tours)));

    if (permutation.back() != this->depot_id || permutation.front() != this->depot_id)
    {
        fitness += JOB_MISSING_PENALTY;
        is_feasible = false;
    }
    penalties.push_back((uint)(permutation.back() != this->depot_id) + (uint)(permutation.front() != this->depot_id));

    if (negative_charge > 0)
    {
        fitness += NEGATIVE_CHARGE_PENALTY + (uint)round(1000 * negative_charge);
        is_feasible = false;
    }
    penalties.push_back((fitness_t)round(abs(negative_charge)));

    delete[] visited;
    return is_feasible;
}

void EVRPInstance::print_nodes()
{
    printf("Car capacity = %u, Battery capacity = %lf, Energy consumption = %lf, Number of tours = %u\n", this->car_capacity, this->battery_capacity, this->energy_consumption, this->tours);
    for (uint i = 0; i < this->node_cnt; i++)
    {
        printf("[%d]: (%d, %d) lb=%u, ub=%u, quant=%u\n",
               i, this->positions[i].x, this->positions[i].y,
               this->lbs[i], this->ubs[i], this->quantities[i]);
    }
}

void EVRPInstance::print_solution(Solution *solution, std::basic_ostream<char> &outf)
{
    uint curr_tour = 0, idx = 0;
    uint cargo_load = this->car_capacity;
    uint unsatisfied = this->total_requests;
    double charge = this->battery_capacity;
    double distance, negative_charge = 0;
    double temp_fitness = 0; //provided optima are floats, this is intended to minimize the rounding error
    bool *visited = new bool[this->node_cnt]{false};
    bool is_feasible = true;
    uint prev_id = solution->permutation[idx++];
    visited[this->depot_id] = true;
    uint cost = 0;

    std::cout.precision(3);
    outf << this->depot_id << "-(" << this->quantities[this->depot_id] <<"/" << cargo_load << "|" << charge << ")";
    for (; idx < solution->permutation.size(); idx++)
    {
        const uint node_id = solution->permutation[idx];
        distance = this->dist_mat(prev_id, node_id);
        temp_fitness += distance;
        charge -= distance * this->energy_consumption;

        if (charge < 0)
        {
            negative_charge += distance * this->energy_consumption;
        }
        if (prev_id == this->depot_id)
            curr_tour++;
        if (node_id == this->depot_id)
        {
            cargo_load = this->car_capacity; // Fill car with cargo
            charge = this->battery_capacity;
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
        if (this->chargers[node_id])
        {
            charge = this->battery_capacity;
        }
        cost = (uint)round(temp_fitness);
        outf << " -(" << cost << ")> " << node_id << "(" << this->quantities[node_id] << "/" << cargo_load << "|" << charge <<")";
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