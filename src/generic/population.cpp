#include "population.hpp"

using std::vector;

Population::Population(uint size, uint penalty_func_cnt, double t_target)
{
    this->size = size;
    this->t_target = t_target;
    this->penalty_func_cnt = penalty_func_cnt;
    this->t_t = vector<double>(this->penalty_func_cnt, 1);
    this->prevSolutions = vector<Solution>(this->size);
}

void Population::update_t_t()
{
    vector<uint> penalty_cnt(this->penalty_func_cnt, 0);
    for (auto solution : this->solutions)
    {
        penalty_cnt[0] += (uint)solution.is_feasible;
        for (uint idx = 1; idx < penalty_cnt.size(); idx++)
        {
            penalty_cnt[idx] += solution.penalties[idx] > 0 ? 1 : 0;
        }
    }
    for (uint idx = 0; idx < penalty_cnt.size(); idx++)
    {
        this->t_t[idx] = (double)penalty_cnt[idx] / this->size;
    }
}

void Population::initPenaltyCoefs()
{
    copy(this->solutions.begin(), this->solutions.end(), this->prevSolutions.begin());
    vector<fitness_t> fitness_stats(this->penalty_func_cnt, 0);
    for (auto solution : this->solutions)
        for (uint i = 0; i < fitness_stats.size(); i++)
            fitness_stats[i] += solution.penalties[i];

    this->penalty_coefficients.push_back(1.);
    for (uint i = 1; i < fitness_stats.size(); i++)
        if (fitness_stats[i] == 0)
            this->penalty_coefficients.push_back(1);
        else
            this->penalty_coefficients.push_back(100 * ((double)fitness_stats[0]) / fitness_stats[i]);
    this->penaltyLimit = *std::max_element(this->penalty_coefficients.begin(), this->penalty_coefficients.end());
}

void Population::update_penalty_coefficients()
{
    double coef = 1.1;

    for (uint idx = 1; idx < this->penalty_func_cnt; idx++)
        if (this->t_t[idx] > this->t_target)
            this->penalty_coefficients[idx] *= coef;
        else
            this->penalty_coefficients[idx] /= coef;

    double min = *std::min_element(this->penalty_coefficients.begin(), this->penalty_coefficients.end());
    double max = *std::max_element(this->penalty_coefficients.begin(), this->penalty_coefficients.end());
    if (max > this->penaltyLimit)
        for (auto &penalty : this->penalty_coefficients)
            penalty = 1 + (penalty - min) * (this->penaltyLimit / max); // linear scaling of penalties to range(1 .. this->penaltyLimit)
    else
        for (auto &penalty : this->penalty_coefficients)
            penalty = 1 + (penalty - min);
}

void Population::resetCheck()
{
    bool allSame = true;
    for (uint j = 0; j < this->size; j++)
    {
        allSame = allSame && (this->prevSolutions[j].permutation == this->solutions[j].permutation);
        this->prevSolutions[j] = this->solutions[j];
    }
    this->is_stalled = allSame;
}

void Population::update_avg_fitness()
{
    if (this->best_known_solution.is_feasible){
        this->avgFitness = 0;
        uint idx = 0;
        for (idx; idx < this->getSize(); idx++){
            if (!this->solutions[idx].is_feasible) break;
            this->avgFitness += this->solutions[idx].fitness;
        }
        this->avgFitness /= idx;
    } else {
        this->avgFitness = INVALID_SOLUTION_FITNESS;
    }
}


void Population::update()
{
    this->update_avg_fitness();
    this->resetCheck();
    this->update_penalty_coefficients();
    this->update_t_t();
}
