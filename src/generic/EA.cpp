#include "EA.hpp"

#include <utility>

using boost::format;
using nlohmann::json;
using permutator::fitness_t;
using std::pair;
using std::string;
using std::vector;

//**********************************************************************************************************************
// INITIALIZATION
//**********************************************************************************************************************

EA::EA(Instance *inst, json config, uint seed)
{
    this->instance = inst;
    this->config = std::move(config);
    std::vector<Solution> pop;
    this->population = pop;
    Solution sol(inst->node_cnt);
    this->best_known_solution = sol;
    this->population_size = this->config["population_size"].get<uint>();
    this->rng = EA::initRng(seed);
    this->t_t = vector<double>(this->instance->penalty_func_cnt, 1);
    this->setConstruction(this->config["construction"].get<string>());
    this->setSelection(this->config["selection"].get<string>());
    this->setCrossover(this->config["crossover"].get<string>());
    // this->setMutation(this->config["mutation"].get<vector<string>>());
    this->mutationList = this->config["mutation"].get<vector<string>>();
    this->setReplacement(this->config["replacement"].get<string>());

    while (this->penalty_coefficients.size() < this->instance->penalty_func_cnt)
        this->penalty_coefficients.push_back(1.);
}

void EA::run()
{
    while (this->population.size() < this->population_size)
        this->population.push_back(this->construction());
    std::uniform_int_distribution<uint> uni(0, this->mutationList.size() - 1);
    for (uint i = 0; !population[0].is_feasible; i++)
    {
        if (i % 10 == 0)
            LOG(i);

        this->update_t_t();
        this->update_penalty_coefficients();
        vector<Solution> parents(this->population_size);
        vector<Solution> children(this->population_size);
        this->constrainedTournament(parents);
        this->crossover(parents, children);
        for (auto &child : children)
            this->mutation_map.at(this->mutationList[uni(*this->rng)])(child);
        this->segregational(children);
    }
    // sort_by_pf(population);
    LOGS(population);
    for (auto p : this->penalty_coefficients)
        LOG(p);
    LOG("--------");
    for (auto t : this->t_t)
        LOG(t);
}

//**********************************************************************************************************************
// CONSTRUCTION
//**********************************************************************************************************************

/*
 * Adds nodes to random positions of the solution, until all nodes are at their LBs.
 * Uses this->initial_solution and sets this->current solution.
 */
Solution EA::constructRandom()
{
    // #if defined STDOUT_ENABLED && STDOUT_ENABLED == 1
    //     this->printOperation(str(format("C: %1%") % __func__));
    // #endif

    // Construct
    Solution sol(this->instance->node_cnt);

    uint rnd_idx;
    for (uint node_id = 0; node_id < this->instance->node_cnt; node_id++)
    { // for all nodes
        while (sol.frequency[node_id] < this->instance->lbs[node_id])
        {
            std::uniform_int_distribution<uint> uni(0, sol.getSize());
            rnd_idx = uni(*this->rng);
            sol.permutation.insert(sol.permutation.begin() + rnd_idx, node_id);
            sol.frequency[node_id]++;
        }
    }
    Solution current_solution = Solution(sol.permutation, *this->instance);

    // Evaluate
    if (current_solution < this->best_known_solution)
    {
        this->best_known_solution = current_solution;
        this->last_improvement = std::chrono::steady_clock::now();
        // this->steps.emplace_back(this->getRuntime(), child.fitness);
    }

    // #if defined STDOUT_ENABLED && STDOUT_ENABLED == 1
    //     this->printResult(true);
    // #endif
    return current_solution;
}

/*
 * Creates a random permutation of nodes.
 * Replicates the permutation, until the solution is not valid w.r.t. LBs and UBs.
 * Uses this->initial_solution and sets this->current solution.
 */
Solution EA::constructRandomReplicate()
{
#if defined STDOUT_ENABLED && STDOUT_ENABLED == 1
    // this->printOperation(str(format("C: %1%") % __func__));
#endif

    // Extract permutation from initial solution
    Solution sol(this->instance->node_cnt);
    auto offset = sol.permutation.size();

    // Append missing nodes
    for (uint i = 0; i < this->instance->node_cnt; i++)
    {
        if (sol.frequency[i] == 0)
        {
            sol.permutation.push_back(i);
            sol.frequency[i]++;
        }
    }

    // Shuffle missing nodes
    auto begin = sol.permutation.begin();
    std::advance(begin, offset);
    std::shuffle(begin, sol.permutation.end(), *this->rng);

    // Replicate, until all nodes are within bounds
    auto perm = sol.permutation;
    sol = Solution(this->instance->node_cnt);
    do
    {
        for (auto elem : perm)
        {
            if (sol.frequency[elem] < this->instance->lbs[elem])
            {
                sol.frequency[elem]++;
                sol.permutation.push_back(elem);
            }
        }
    } while (!this->instance->FrequencyInBounds(sol.frequency) /*&& !this->stop()*/);

    // Evaluate
    Solution current_solution = Solution(sol.permutation, *this->instance);
    if (current_solution < this->best_known_solution)
    {
        this->best_known_solution = current_solution;
        this->last_improvement = std::chrono::steady_clock::now();
        // this->steps.emplace_back(this->getRuntime(), child.fitness);
    }

#if defined STDOUT_ENABLED && STDOUT_ENABLED == 1
    // this->printResult(true);
#endif
    return current_solution;
}

//**********************************************************************************************************************
// SELECTION
//**********************************************************************************************************************

void EA::constrainedTournament(vector<Solution> &parents)
{

    // vector<Solution> parents(parent_cnt);
    uint tournament_size = 4;
    vector<Solution> tournament(tournament_size);
    vector<Solution> feasibleSolutions(0);
    vector<Solution> source_population;
    // vector<uint> feasibleIdxs(0);
    // for (uint idx = 0; idx < this->population.size(); idx++){
    //     if (this->population[idx].is_feasible) feasibleIdxs.push_back(idx);
    // }
    for (auto p : population)
        if (p.is_feasible)
            feasibleSolutions.push_back(p);

    for (uint parent_idx = 0; parent_idx < parents.size(); parent_idx++)
    {
        if (this->t_t[0] < this->t_target && feasibleSolutions.size() > 1 && parent_idx % 2 == 1 && parents[parent_idx - 1].is_feasible)
            source_population = feasibleSolutions;
        else
            source_population = population;

        for (uint tournament_idx = 0; tournament_idx < tournament_size; tournament_idx++)
        {
            std::uniform_int_distribution<uint> uni(0, source_population.size() - 1);
            uint rnd_idx = uni(*this->rng);
            tournament[tournament_idx] = source_population[rnd_idx];
        }
        sort_by_pf(tournament);
        parents[parent_idx] = tournament[0];
    }
}

//**********************************************************************************************************************
// CROSSOVER
//**********************************************************************************************************************

void EA::swapNode(std::vector<Solution> parents, std::vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {

        vector<uint> perm1 = parents[idx].permutation;
        vector<uint> perm2 = parents[idx + 1].permutation;
        vector<uint> new_perm = perm1;
        vector<uint> removed_nodes;
        std::uniform_int_distribution<uint> uni(0, this->instance->node_cnt - 1);

        // choosing nodes to be removed from perm1 and perm2
        for (int i = 0; i < this->instance->node_cnt / 5; i++)
            removed_nodes.push_back(uni(*this->rng));
        // removing chosen nodes from perm1
        for (auto node : removed_nodes)
        {
            auto node_removed_end = std::remove(new_perm.begin(), new_perm.end(), node);
            new_perm.erase(node_removed_end, new_perm.end());
        }
        // inserting removed nodes on indexes as in perm 2
        for (uint i = 0; i < perm2.size(); i++)
        {
            if (std::count(removed_nodes.begin(), removed_nodes.end(), perm2[i]))
            {
                auto it = i < new_perm.size() ? new_perm.begin() + i : new_perm.end();
                new_perm.insert(it, perm2[i]);
            }
        }
        children[idx] = Solution(new_perm, *this->instance);
        new_perm = perm2;
        // removing chosen nodes from perm2
        for (auto node : removed_nodes)
        {
            auto node_removed_end = std::remove(new_perm.begin(), new_perm.end(), node);
            new_perm.erase(node_removed_end, new_perm.end());
        }
        // inserting removed nodes on indexes as in perm1
        for (uint i = 0; i < perm1.size(); i++)
        {
            if (std::count(removed_nodes.begin(), removed_nodes.end(), perm1[i]))
            {
                auto it = i < new_perm.size() ? new_perm.begin() + i : new_perm.end();
                new_perm.insert(it, perm1[i]);
            }
        }
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void EA::OX(std::vector<Solution> parents, std::vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {

        vector<uint> perm1 = parents[idx].permutation;
        vector<uint> perm2 = parents[idx + 1].permutation;
        vector<uint> new_perm = perm1;
        vector<uint> removed_nodes;
        std::uniform_int_distribution<uint> uni(0, this->instance->node_cnt - 1);
    }
}

//**********************************************************************************************************************
// MUTATION
//**********************************************************************************************************************

void EA::insert(Solution &child)
{

    Solution best_solution = child;
    vector<uint> perm = child.permutation;
    vector<uint> freq = child.frequency;
    fitness_t new_fitness;
    vector<fitness_t> new_penalties;
    bool updated = false;

    for (uint i = 0; i <= perm.size(); i++)
    { // for all positions i in X
        vector<uint> new_perm = perm;
        vector<uint> new_freq = freq;
        new_perm.insert(new_perm.begin() + i, 0);

        for (uint j = 0; j < this->instance->node_cnt; j++)
        { // for all nodes j in A
            if (child.frequency[j] >= this->instance->ubs[j])
                continue;
            new_perm[i] = j;
            new_freq[j]++;
            this->instance->computeFitness(new_perm, new_fitness, new_penalties);

            if (this->penalized_fitness(new_penalties) < this->penalized_fitness(best_solution))
            {
                best_solution = Solution(new_perm, *this->instance);
                updated = true;
            }
            new_freq[j]--;
        }
    }
    child = best_solution;
}

void EA::centeredExchange(uint x, Solution &child)
{

    fitness_t fitness;
    vector<fitness_t> penalties;
    if (x > child.permutation.size())
        return;
    vector<uint> perm = child.permutation;
    Solution best_solution = child;
    bool updated = false;

    for (int i = (int)x; i < (int)perm.size() - (int)x; i++)
    {
        vector<uint> new_perm = perm;
        for (int j = 1; j <= (int)x; j++)
        {
            new_perm[i + j] = perm[i - j];
            new_perm[i - j] = perm[i + j];
        }
        this->instance->computeFitness(new_perm, fitness, penalties);
        if (this->penalized_fitness(penalties) < this->penalized_fitness(best_known_solution))
        {
            best_solution = Solution(new_perm, *this->instance);
            updated = true;
        }
    }

    child = best_solution;
}

void EA::relocate(uint x, bool reverse, Solution &child)
{

    if (x > child.permutation.size())
        return;

    fitness_t fitness;
    vector<fitness_t> penalties;
    vector<uint> perm = child.permutation;
    Solution best_solution = child;
    bool updated = false;

    for (uint i = 0; i < perm.size() - x; i++)
    {
        vector<uint> reduced_perm = perm;
        vector<uint> subsequence(perm.begin() + i, perm.begin() + i + x);
        reduced_perm.erase(reduced_perm.begin() + i, reduced_perm.begin() + i + x);
        for (uint j = 0; j < reduced_perm.size(); j++)
        {
            vector<uint> new_perm = reduced_perm;
            if (reverse)
            {
                new_perm.insert(new_perm.begin() + j, subsequence.rbegin(), subsequence.rend());
            }
            else
            {
                new_perm.insert(new_perm.begin() + j, subsequence.begin(), subsequence.end());
            }
            this->instance->computeFitness(new_perm, fitness, penalties);
            if (this->penalized_fitness(penalties) < this->penalized_fitness(best_solution))
            {
                best_solution = Solution(new_perm, *this->instance);
                updated = true;
            }
        }
    }
    child = best_solution;
}

void EA::remove(Solution &child)
{

    if (child.permutation.empty())
        return;
    vector<uint> perm = child.permutation;
    fitness_t fitness;
    vector<fitness_t> penalties;
    Solution best_solution = child;
    bool updated = false;

    for (uint i = 0; i < perm.size(); i++)
    {
        if (child.frequency[perm[i]] <= this->instance->lbs[perm[i]])
            continue;

        vector<uint> new_perm = perm;
        new_perm.erase(new_perm.begin() + i);
        this->instance->computeFitness(new_perm, fitness, penalties);
        if (this->penalized_fitness(penalties) <= this->penalized_fitness(best_solution))
        {
            best_solution = Solution(new_perm, *this->instance);
            updated = true;
        }
    }
    child = best_solution;
}

//**********************************************************************************************************************
// REPLACEMENT
//**********************************************************************************************************************

void EA::segregational(std::vector<Solution> children)
{
    // this->population.clear();
    // parents.insert(parents.end(), children.begin(), children.end());
    vector<Solution> validSolutions(0), rest(0);

    std::copy_if(this->population.begin(), this->population.end(), std::back_inserter(validSolutions), [](Solution s)
                 { return s.is_feasible; });
    std::copy_if(children.begin(), children.end(), std::back_inserter(validSolutions), [](Solution s)
                 { return s.is_feasible; });
    std::copy_if(this->population.begin(), this->population.end(), std::back_inserter(rest), [](Solution s)
                 { return !s.is_feasible; });
    std::copy_if(children.begin(), children.end(), std::back_inserter(rest), [](Solution s)
                 { return !s.is_feasible; });

    this->population.clear();
    sort_by_pf(validSolutions);
    for (uint idx = 0; idx < validSolutions.size(); idx++)
    {
        if (this->population.size() < this->population_size * t_select &&
            find(this->population.begin(), this->population.end(), validSolutions[idx]) == this->population.end())
        {
            this->population.push_back(validSolutions[idx]);
        }
        else
        {
            rest.push_back(validSolutions[idx]);
        }
    }

    sort_by_pf(rest);
    uint idx = 0;
    while (this->population.size() < this->population_size)
    {
        for (; idx < rest.size(); idx++)
        {
            if (this->population.size() < this->population_size && find(this->population.begin(), this->population.end(), rest[idx]) == this->population.end())
                this->population.push_back(rest[idx]);
        }
        this->population.push_back(this->construction());
    }
    this->best_known_solution = this->population[0];
}

//**********************************************************************************************************************
// SET OPERATIONS
//**********************************************************************************************************************

void EA::setConstruction(const string &constr)
{
    if (constr == "random")
        this->construction = [this]
        { return this->constructRandom(); };
    else if (constr == "randomReplicate")
        this->construction = [this]
        { return this->constructRandomReplicate(); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void EA::setSelection(const string &constr)
{
    if (constr == "constrainedTournament")
        this->selection = [this](std::vector<Solution> &parents)
        { return constrainedTournament(parents); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void EA::setCrossover(const string &constr)
{
    if (constr == "swapNode")
        this->crossover = [this](std::vector<Solution> parents, std::vector<Solution> &children)
        { return this->swapNode(parents, children); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void EA::setReplacement(const string &constr)
{
    if (constr == "segregational")
        this->replacement = [this](std::vector<Solution> children)
        { return segregational(children); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

//**********************************************************************************************************************
// UTILS
//**********************************************************************************************************************

std::mt19937 *EA::initRng(uint seed)
{
    if (seed == 0)
    {
        std::random_device rd;
        seed = rd();
    }
    return new std::mt19937(seed);
}

void EA::update_t_t()
{
    vector<uint> penalty_cnt(this->instance->penalty_func_cnt, 0);
    for (auto solution : this->population)
    {
        penalty_cnt[0] += solution.is_feasible;
        for (uint idx = 1; idx < penalty_cnt.size(); idx++)
        {
            penalty_cnt[idx] += solution.penalties[idx] > 0 ? 1 : 0;
        }
    }
    for (uint idx = 0; idx < penalty_cnt.size(); idx++)
    {
        this->t_t[idx] = (double)penalty_cnt[idx] / this->population_size;
    }
}

void EA::update_penalty_coefficients()
{
    double coef;
    for (uint idx = 1; idx < this->instance->penalty_func_cnt; idx++)
    {
        coef = this->t_t[idx] > this->t_target ? 1.1 : 0.9;
        this->penalty_coefficients[idx] *= coef;
    }
}

void EA::sort_by_pf(vector<Solution> &v)
{

    std::sort(v.begin(), v.end(),
              [this](Solution A, Solution B) -> bool
              {
                  return this->penalized_fitness(A) < this->penalized_fitness(B);
              });
}

fitness_t EA::penalized_fitness(Solution solution)
{
    return (fitness_t)round(inner_product(solution.penalties.begin(), solution.penalties.end(), this->penalty_coefficients.begin(), 0.));
}

fitness_t EA::penalized_fitness(std::vector<fitness_t> penalties)
{
    return (fitness_t)round(inner_product(penalties.begin(), penalties.end(), this->penalty_coefficients.begin(), 0.));
}
