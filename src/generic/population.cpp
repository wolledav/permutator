#include "population.hpp"

using std::vector;

Population::Population(uint size, uint penalty_func_cnt, double t_target)
{
    this->size = size;
    this->t_target = t_target;
    this->penalty_func_cnt = penalty_func_cnt;
    this->t_t = vector<double>(this->penalty_func_cnt, 1);
    this->last_gen_solutions = vector<Solution>(this->size);
}

void Population::initializePenalty()
{
    copy(this->solutions.begin(), this->solutions.end(), this->last_gen_solutions.begin());
    vector<fitness_t> fitness_stats(this->penalty_func_cnt, 0);

    for (auto solution : this->solutions)
        for (uint idx = 0; idx < fitness_stats.size(); idx++)
            fitness_stats[idx] += solution.penalties[idx];

    this->penalty.push_back(1.);
    for (uint idx = 1; idx < fitness_stats.size(); idx++)
        if (fitness_stats[idx] == 0)
            this->penalty.push_back(1);
        else
            this->penalty.push_back(100 * ((double)fitness_stats[0]) / fitness_stats[idx]);

    double min = *std::min_element(this->penalty.begin(), this->penalty.end());
    double max = *std::max_element(this->penalty.begin(), this->penalty.end());
    this->penalty_ceiling = std::max(*std::max_element(this->penalty.begin(), this->penalty.end()), PENALTY_CEILING);
    
    for (auto &p : this->penalty)
            p = 1 + (p - min) * (this->penalty_ceiling / max);
}

void Population::updatePenalty()
{
    for (uint idx = 1; idx < this->penalty_func_cnt; idx++)
        if (this->t_t[idx] > this->t_target)
            this->penalty[idx] *= GROWTH_COEFFICIENT;
        else
            this->penalty[idx] /= GROWTH_COEFFICIENT;

    double min = *std::min_element(this->penalty.begin(), this->penalty.end());
    double max = *std::max_element(this->penalty.begin(), this->penalty.end());

    if (max > this->penalty_ceiling)
        for (auto &p : this->penalty)
            p = 1 + (p - min) * (this->penalty_ceiling / max); // linear scaling of penalties to [1.0 ... this->penalty_ceiling]
    else
        for (auto &p : this->penalty)
            p = 1 + (p - min);
        
}

void Population::update_t_t()
{
    vector<uint> penalty_violation_cnt(this->penalty_func_cnt, 0);
    for (auto solution : this->solutions)
    {
        penalty_violation_cnt[0] += (uint)solution.is_feasible;
        for (uint idx = 1; idx < penalty_violation_cnt.size(); idx++)
            penalty_violation_cnt[idx] += solution.penalties[idx] > 0 ? 1 : 0;        
    }
    for (uint idx = 0; idx < penalty_violation_cnt.size(); idx++)
        this->t_t[idx] = (double)penalty_violation_cnt[idx] / this->size;
}


void Population::stallPopulationCheck()
{
    bool all_solutions_same = true;
    for (uint j = 0; j < this->size; j++)
    {
        all_solutions_same = all_solutions_same && (this->last_gen_solutions[j].permutation == this->solutions[j].permutation);
        this->last_gen_solutions[j] = this->solutions[j];
    }
    this->is_stalled = all_solutions_same;
}

void Population::updateAvgFitness()
{
    if (this->best_known_solution.is_feasible)
    {
        this->avg_fitness = 0;
        uint idx = 0;
        for (idx; idx < this->getRealSize(); idx++)
        {
            if (this->solutions[idx].is_feasible)
                this->avg_fitness += this->solutions[idx].fitness;
            else
                break;
        }
        this->avg_fitness /= idx;
    }
    else
    {
        this->avg_fitness = MAX_FITNESS;
    }
}

void Population::update()
{
    this->best_known_solution = this->solutions[0];
    this->updateAvgFitness();
    this->stallPopulationCheck();
    this->update_t_t();
    this->updatePenalty();
}

uint Population::getRealSize()
{
    return this->solutions.size();
}

uint Population::getMaxSize()
{
    return this->size;
}

void Population::append(Solution solution)
{
    this->solutions.push_back(solution);
}

void Population::clear()
{
    this->solutions.clear();
}

void Population::print()
{
    for (int i = solutions.size() - 1; i >= 0; i--)
        solutions[i].print();
    std::cout << "\npopulation size: " << this->size << " | generation: " << this->generation << std::endl << std::endl;
}

vector<double> Population::get_penalty()
{
    return this->penalty;
}

vector<double> Population::get_t_t()
{
    return this->t_t;
}

double Population::get_t_target()
{
    return this->t_target;
}