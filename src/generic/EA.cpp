
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
    this->populations[0] = new Population(this->config["population_size"].get<uint>(), this->instance->penalty_func_cnt);
    this->population = this->populations[0];
    this->best_known_solution = Solution(inst->node_cnt);
    this->timeout_s = this->config["timeout"].get<uint>();
    this->frequency = this->config["frequency"].get<uint>();
    this->rng = EA::initRng(seed);

    this->setConstruction(this->config["construction"].get<string>());
    this->setSelection(this->config["selection"].get<string>());
    this->setCrossover(this->config["crossover"].get<string>());
    this->mutationList = this->config["mutation"].get<vector<string>>();
    this->setReplacement(this->config["replacement"].get<string>());
}

void EA::run()
{
    this->start = std::chrono::steady_clock::now();
    this->construction();
    this->population->initPenaltyCoefs();

    uint gen = 0;
    while (!this->stop())
    {
        this->changePopulation();

        vector<Solution> parents(this->population->size);
        vector<Solution> children(this->population->size);

        this->constrainedTournament(parents);
        this->crossover(parents, children);
        this->mutation(children);
        this->replacement(children);
        this->population->update();
        gen++;
    }
    
    Population* bestPop = *min_element(this->populations.begin(), this->populations.end(), [](Population* a, Population* b){return a->best_known_solution.fitness < b->best_known_solution.fitness;});
    this->population = bestPop;
    this->population->print();
    std::cout << "runtime: " << this->getRuntime() << "/" << this->timeout_s << "s" << std::endl;
    LOG(gen);
}

//**********************************************************************************************************************
// CONSTRUCTION
//**********************************************************************************************************************

/*
 * Adds nodes to random positions of the solution, until all nodes are at their LBs.
 * Uses this->initial_solution and sets this->current solution.
 */
void EA::constructRandom()
{
    while (this->population->getSize() < this->population->size)
    {
        if (this->stop())
            return;
        Solution sol(this->instance->node_cnt);
        construct::random(sol.permutation, sol.frequency, this->instance->lbs, this->rng);
        sol.is_feasible = this->instance->computeFitness(sol.permutation, sol.fitness, sol.penalties);
        this->population->append(sol);
    }
}

/*
 * Creates a random permutation of nodes.
 * Replicates the permutation, until the solution is not valid w.r.t. LBs and UBs.
 * Uses this->initial_solution and sets this->current solution.
 */
void EA::constructRandomReplicate()
{
    while (this->population->getSize() < this->population->size)
    {
        if (this->stop())
            return;
        Solution sol(this->instance->node_cnt);
        construct::randomReplicate(sol.permutation, sol.frequency, this->instance->lbs, this->instance->ubs, this->rng);
        sol.is_feasible = this->instance->computeFitness(sol.permutation, sol.fitness, sol.penalties);
        this->population->append(sol);
    }
}

//**********************************************************************************************************************
// SELECTION
//**********************************************************************************************************************

void EA::constrainedTournament(vector<Solution> &parents)
{
    sort_by_pf(this->population->solutions);
    this->constrainedSortedTournament(parents);
}

void EA::constrainedSortedTournament(vector<Solution> &parents)
{

    uint tournament_size = 4;
    vector<uint> tournament(tournament_size);
    vector<Solution> feasibleSolutions(0);
    vector<Solution> source_population;

    for (auto p : this->population->solutions)
        if (p.is_feasible)
            feasibleSolutions.push_back(p);

    for (uint parent_idx = 0; parent_idx < parents.size(); parent_idx++)
    {
        if (this->stop())
            return;
        if (this->population->get_t_t()[0] < this->population->get_t_target() && feasibleSolutions.size() > 1 && parent_idx % 2 == 1 && parents[parent_idx - 1].is_feasible)
            source_population = feasibleSolutions;
        else
            source_population = this->population->solutions;

        std::uniform_int_distribution<uint> rnd_idx(0, source_population.size() - 1);
        for (uint tournament_idx = 0; tournament_idx < tournament_size; tournament_idx++)
            tournament[tournament_idx] = rnd_idx(*this->rng);

        parents[parent_idx] = this->population->solutions[*min_element(tournament.begin(), tournament.end())];
    }
}

//**********************************************************************************************************************
// CROSSOVER
//**********************************************************************************************************************

void EA::insertNode(vector<Solution> parents, vector<Solution> &children, uint x)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> perm1 = parents[idx].permutation;
        vector<uint> perm2 = parents[idx + 1].permutation;
        vector<uint> new_perm;
        vector<uint> removed_nodes;
        std::uniform_int_distribution<uint> uni(0, this->instance->node_cnt - 1);

        // choosing nodes to be removed from perm1 and perm2
        for (int i = 0; i < (this->instance->node_cnt * x) / 100; i++)
            removed_nodes.push_back(uni(*this->rng));

        crossover::insertNode(perm1, perm2, new_perm, removed_nodes);
        children[idx] = Solution(new_perm, *this->instance);

        crossover::insertNode(perm2, perm1, new_perm, removed_nodes);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void EA::ERX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> perm1 = parents[idx].permutation;
        vector<uint> perm2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::ERX(perm1, perm2, new_perm, this->instance->lbs, this->instance->ubs, this->rng);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::ERX(perm2, perm1, new_perm, this->instance->lbs, this->instance->ubs, this->rng);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

//**********************************************************************************************************************
// MUTATION
//**********************************************************************************************************************

void EA::mutation(vector<Solution> &children)
{
    std::uniform_int_distribution<uint> randMutation(0, this->mutationList.size() - 1);
    for (auto &child : children)
    {
        if (this->stop())
            return;
        this->mutation_map.at(this->mutationList[randMutation(*this->rng)])(child);
    }
}

void EA::insert(Solution &child)
{
    // LOG(__func__);
    vector<uint> possibleNodes(0);
    for (uint i = 0; i < this->instance->node_cnt; i++)
        if (child.frequency[i] < this->instance->ubs[i])
            possibleNodes.push_back(i);

    if (possibleNodes.size() == 0)
        return;

    std::uniform_int_distribution<uint> randNodeIdx(0, possibleNodes.size() - 1);
    std::uniform_int_distribution<uint> randPosition(0, child.permutation.size());

    oprtr::insert(child.permutation, child.frequency, this->instance->ubs, possibleNodes[randNodeIdx(*this->rng)], randPosition(*this->rng));
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::append(Solution &child)
{
    vector<uint> possibleNodes(0);
    for (uint i = 0; i < this->instance->node_cnt; i++)
        if (child.frequency[i] < this->instance->ubs[i])
            possibleNodes.push_back(i);

    if (possibleNodes.size() == 0)
        return;

    std::uniform_int_distribution<uint> randNodeIdx(0, possibleNodes.size() - 1);
    oprtr::append(child.permutation, child.frequency, this->instance->ubs, possibleNodes[randNodeIdx(*this->rng)]);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::remove(Solution &child)
{
    vector<uint> possiblePositions(0);
    for (uint i = 0; i < child.permutation.size(); i++)
    {
        uint node = child.permutation[i];
        if (child.frequency[node] > this->instance->lbs[node])
            possiblePositions.push_back(i);
    }
    if (possiblePositions.size() == 0)
        return;

    std::uniform_int_distribution<uint> randPositionIdx(0, possiblePositions.size() - 1);
    oprtr::remove(child.permutation, child.frequency, this->instance->lbs, possiblePositions[randPositionIdx(*this->rng)]);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::centeredExchange(Solution &child, uint x)
{
    std::uniform_int_distribution<uint> randPosition(x, child.permutation.size() - x - 1);
    oprtr::centeredExchange(child.permutation, randPosition(*this->rng), x);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::relocate(Solution &child, uint x, bool reverse)
{
    std::uniform_int_distribution<uint> randFrom(0, child.permutation.size() - x);
    std::uniform_int_distribution<uint> randTo(0, child.permutation.size() - x);
    oprtr::relocate(child.permutation, randFrom(*this->rng), randTo(*this->rng), x, reverse);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::exchange(Solution &child, uint x, uint y, bool reverse)
{
    std::uniform_int_distribution<uint> posX(0, child.permutation.size() - x);
    std::uniform_int_distribution<uint> posY(0, child.permutation.size() - y);

    oprtr::exchange(child.permutation, posX(*this->rng), posY(*this->rng), x, y, reverse);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}
void EA::moveAll(Solution &child, uint x)
{
    std::uniform_int_distribution<uint> randomNode(0, this->instance->node_cnt - 1);
    std::uniform_int_distribution<int> randomOffset(-x, x);
    oprtr::moveAll(child.permutation, randomNode(*this->rng), randomOffset(*this->rng));
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::exchangeIds(Solution &child)
{
    std::uniform_int_distribution<uint> randomNode(0, this->instance->node_cnt - 1);
    uint nodeX, nodeY;
    do
    {
        nodeX = randomNode(*this->rng);
        nodeY = randomNode(*this->rng);
    } while (nodeX != nodeY);

    oprtr::exchangeIds(child.permutation, child.frequency, this->instance->lbs, this->instance->ubs, nodeX, nodeY);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::twoOpt(Solution &child)
{
    std::uniform_int_distribution<uint> randPosition(0, child.permutation.size() - 1);
    uint posA, posB;
    do
    {
        posA = randPosition(*this->rng);
        posB = randPosition(*this->rng);
    } while (posA != posB);

    oprtr::reverse(child.permutation, std::min(posA, posB), std::abs((int)posA - (int)posB));
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

//**********************************************************************************************************************
// REPLACEMENT
//**********************************************************************************************************************

void EA::nicheSegregational(vector<Solution> children)
{
    if (this->stop())
        return;

    copy(this->population->solutions.begin(), this->population->solutions.end(), std::back_inserter(children));
    sort_by_pf(children);

    vector<bool> leaderMask(0), followerMask(0);
    this->nichePopulation(children, 1, leaderMask, followerMask);
    int leaderFollowerBalance = 0;

    vector<uint> feasibleIdxs(0), restIdxs(0);

    for (uint i = 0; i < children.size(); i++)
    {
        if (children[i].is_feasible)
            feasibleIdxs.push_back(i);
        else
            restIdxs.push_back(i);
    }

    this->population->clear();

    for (auto mask : {leaderMask, followerMask})
    {
        for (auto idx : feasibleIdxs)
        {
            if (!mask[idx])
                continue;
            if (this->population->getSize() < this->population->size * t_select)
            {
                if (true || find(this->population->solutions.begin(), this->population->solutions.end(), children[idx]) == this->population->solutions.end())
                {
                    this->population->append(children[idx]);
                    leaderFollowerBalance += leaderMask[idx] ? 1 : -1;
                }
            }
            else
            {
                restIdxs.push_back(idx);
            }
        }
    }

    sort(restIdxs.begin(), restIdxs.end());

    for (auto mask : {leaderMask, followerMask})
    {
        for (auto idx : restIdxs)
            if (mask[idx] && this->population->getSize() < this->population->size && (true || find(this->population->solutions.begin(), this->population->solutions.end(), children[idx]) == this->population->solutions.end()))
            {
                this->population->append(children[idx]);
                leaderFollowerBalance += leaderMask[idx] ? 1 : -1;
            }
    }
    this->construction();

    this->population->best_known_solution = this->population->solutions[0];
    this->best_known_solution = this->best_known_solution.fitness <  this->population->solutions[0].fitness ? this->best_known_solution : this->population->solutions[0];

    // adaptive niche radius
    if (leaderFollowerBalance > 0)
        this->nicheRadius += 1;
    else if (leaderFollowerBalance < 0)
        this->nicheRadius -= 1;
}

void EA::segregational(vector<Solution> children)
{
    if (this->stop())
        return;

    copy(this->population->solutions.begin(), this->population->solutions.end(), std::back_inserter(children));
    sort_by_pf(children);
    vector<uint> feasibleIdxs(0), restIdxs(0);

    for (uint i = 0; i < children.size(); i++)
    {
        if (children[i].is_feasible)
            feasibleIdxs.push_back(i);
        else
            restIdxs.push_back(i);
    }

    this->population->clear();

    for (auto idx : feasibleIdxs)
    {
        if (this->population->getSize() < this->population->size * t_select)
        {
            if (find(this->population->solutions.begin(), this->population->solutions.end(), children[idx]) == this->population->solutions.end())
                this->population->append(children[idx]);
        }
        else
        {
            restIdxs.push_back(idx);
        }
    }

    sort(restIdxs.begin(), restIdxs.end());

    // if solution exist with some part of the penalty equal to zero, it wont disappear from the population
    if (this->population->getSize() == 0)
    { // if size > 0 then feasible solution exist and its penalty vector == 0
        vector<bool> noPenalty(this->instance->penalty_func_cnt, true);
        for (auto idx : restIdxs)
            for (uint i = 0; i < noPenalty.size(); i++)
                if (noPenalty[i] && children[idx].penalties[i] == 0)
                {
                    this->population->append(children[idx]);
                    noPenalty[i] = false;
                }
    }

    for (auto idx : restIdxs)
        if (this->population->getSize() < this->population->size && find(this->population->solutions.begin(), this->population->solutions.end(), children[idx]) == this->population->solutions.end())
            this->population->append(children[idx]);

    this->construction();

    this->population->best_known_solution = this->population->solutions[0];
    this->best_known_solution = this->best_known_solution.fitness <  this->population->solutions[0].fitness ? this->best_known_solution : this->population->solutions[0];
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
        this->selection = [this](vector<Solution> &parents)
        { return constrainedTournament(parents); };
    else if (constr == "constrainedSortedTournament")
        this->selection = [this](vector<Solution> &parents)
        { return constrainedSortedTournament(parents); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void EA::setCrossover(const string &constr)
{
    if (constr == "insertNode_5")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->insertNode(parents, children, 5); };
    else if (constr == "insertNode_10")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->insertNode(parents, children, 10); };
    else if (constr == "insertNode_20")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->insertNode(parents, children, 20); };
    else if (constr == "insertNode_50")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->insertNode(parents, children, 50); };
    else if (constr == "ERX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->ERX(parents, children); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void EA::setReplacement(const string &constr)
{
    if (constr == "segregational")
        this->replacement = [this](vector<Solution> children)
        { return segregational(children); };
    else if (constr == "nicheSegregational")
        this->replacement = [this](vector<Solution> children)
        { return nicheSegregational(children); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

//**********************************************************************************************************************
// UTILS
//**********************************************************************************************************************

void EA::nichePopulation(vector<Solution> population, uint capacity, vector<bool> &leaderMask, vector<bool> &followerMask)
{
    leaderMask = vector<bool>(population.size(), false);
    followerMask = vector<bool>(population.size(), false);

    for (uint i = 0; i < population.size(); i++)
    {
        if (!followerMask[i])
        {
            leaderMask[i] = true;
            uint occupancy = 1;
            for (uint j = i + 1; j < population.size(); j++)
            {
                if (!followerMask[j] && levenshtein_distance(population[i].permutation, population[j].permutation) <= this->nicheRadius)
                {
                    if (occupancy < capacity)
                    {
                        leaderMask[j] = true;
                        occupancy += 1;
                    }
                    else
                    {
                        followerMask[j] = true;
                    }
                }
            }
        }
    }
}

std::mt19937 *EA::initRng(uint seed)
{
    if (seed == 0)
    {
        std::random_device rd;
        seed = rd();
    }
    return new std::mt19937(seed);
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
    return (fitness_t)round(inner_product(solution.penalties.begin(), solution.penalties.end(), this->population->get_penalty_coefficients().begin(), 0.));
}

fitness_t EA::penalized_fitness(vector<fitness_t> penalties)
{
    return (fitness_t)round(inner_product(penalties.begin(), penalties.end(), this->population->get_penalty_coefficients().begin(), 0.));
}

bool EA::stop()
{
    return this->getRuntime() > this->timeout_s ||
           (config["stop_on_feasible"] && this->best_known_solution.is_feasible);
}

void EA::changePopulation()
{
    //choose the next population to run
    int activePopIdx = -1;
    do {
        activePopIdx++;
        this->counter[activePopIdx] = (this->counter[activePopIdx] + 1) % this->frequency;
    } while (this->counter[activePopIdx] == 0);
    if (this->populations[activePopIdx] != nullptr) {
        this->population = this->populations[activePopIdx];
    }
    else { 
        this->populations[activePopIdx] = new Population((this->populations[activePopIdx - 1]->size) * 2, this->instance->penalty_func_cnt);
        this->population = this->populations[activePopIdx];
        this->construction();
        this->population->initPenaltyCoefs();
    }

    this->population->generation++;

    //deletes worse or stalled populations
    if (this->population->is_stalled && this->populations[activePopIdx + 1] != nullptr){
        std::cout << "---------erased: " << activePopIdx << "|" << this->population->size << " for stall---------" << std::endl;
        this->populations.erase(this->populations.begin() + activePopIdx);     
        this->counter.erase(this->counter.begin() + activePopIdx);
        this->changePopulation();
    } 
    for (uint idx2 = 0; idx2 < activePopIdx; idx2++){
        if (this->populations[activePopIdx]->avgFitness < this->populations[idx2]->avgFitness){
            std::cout << "--------erased: " <<  idx2 << "|" << this->populations[idx2]->size << " for lower avg fitness---" <<  this->populations[activePopIdx]->avgFitness << "|" << this->populations[idx2]->avgFitness << "---------" << std::endl;
            this->populations.erase(this->populations.begin() + idx2);
            this->counter.erase(this->counter.begin() + activePopIdx);
            activePopIdx -= 1;    
        }
    }
}

void EA::saveToJson(json &container)
{
}