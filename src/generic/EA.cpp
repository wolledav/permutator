
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
    this->population = vector<Solution>(0);
    this->best_known_solution = Solution(inst->node_cnt);
    this->population_size = this->config["population_size"].get<uint>();
    this->timeout_s = this->config["timeout"].get<uint>();
    this->rng = EA::initRng(seed);
    this->t_t = vector<double>(this->instance->penalty_func_cnt, 1);
    this->setConstruction(this->config["construction"].get<string>());
    this->setSelection(this->config["selection"].get<string>());
    this->setCrossover(this->config["crossover"].get<string>());
    this->mutationList = this->config["mutation"].get<vector<string>>();
    this->setReplacement(this->config["replacement"].get<string>());
    this->lastPop = vector<Solution>(this->population_size);
}


void EA::run()
{
    this->start = std::chrono::steady_clock::now();

    this->construction();
    copy(this->population.begin(), this->population.end(), this->lastPop.begin());
    this->initPenaltyCoefs();

    uint gen = 0;
    while (!this->stop())
    {   
        // if (gen%10 == 0){
        //     LOG(gen);
        //     std::cout << this->getRuntime() << "/" << this->timeout_s << "s" << std::endl;
        //     LOG("--");
        //     for (auto p : this->penalty_coefficients)
        //         LOG(p);
            // LOG("--");
            // for (auto t : this->t_t)
            //     LOG(t);
            // LOG("--------");
        //     //if (i%100 == 0) LOGS(population);
        // }
 
        vector<Solution> parents(this->population_size);
        vector<Solution> children(this->population_size);
        
        this->constrainedTournament(parents);
        this->crossover(parents, children);
        this->mutation(children);
        this->segregational(children);
        this->populationReset();
        this->update_t_t();
        this->update_penalty_coefficients(); 
        gen++;
    }
    {
    // LOGS(this->population);
    // for (auto p : this->penalty_coefficients)
    //     LOG(p);
    // LOG("--------");
    // for (auto t : this->t_t)
    //     LOG(t);
    // LOG("--------");
    // std::cout << "generation: " << gen << std::endl;
    std::cout << "runtime: " << this->getRuntime() << "/" << this->timeout_s << "s" << std::endl;
    }
}


void EA::populationReset(){
    if (this->stop()) return;
    bool allSame = true; 
    for (uint j = 0; j < this->population_size; j++){
        allSame = allSame & (this->lastPop[j].permutation == this->population[j].permutation);
        this->lastPop[j] = this->population[j];
    }

    if (allSame) {
        // LOG("------------------------population reset-----------------------------");       
        this->population.erase(this->population.begin() + this->instance->penalty_func_cnt, this->population.end());
        this->construction();
        copy(this->population.begin(), this->population.end(), this->lastPop.begin());
    } 
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
    while (this->population.size() < this->population_size) {
        if (this->stop()) return;
        Solution sol(this->instance->node_cnt);
        construct::random(sol.permutation, sol.frequency, this->instance->lbs, this->rng);
        sol.is_feasible = this->instance->computeFitness(sol.permutation, sol.fitness, sol.penalties);
        this->population.push_back(sol);
    }
}

/*
 * Creates a random permutation of nodes.
 * Replicates the permutation, until the solution is not valid w.r.t. LBs and UBs.
 * Uses this->initial_solution and sets this->current solution.
 */
void EA::constructRandomReplicate()
{
    while (this->population.size() < this->population_size) {
        if (this->stop()) return;
        Solution sol(this->instance->node_cnt);
        construct::randomReplicate(sol.permutation, sol.frequency, this->instance->lbs, this->instance->ubs, this->rng);
        sol.is_feasible = this->instance->computeFitness(sol.permutation, sol.fitness, sol.penalties);
        this->population.push_back(sol);
    }
}

//**********************************************************************************************************************
// SELECTION
//**********************************************************************************************************************

void EA::constrainedTournament(vector<Solution> &parents)
{

    uint tournament_size = 4;
    vector<Solution> tournament(tournament_size);
    vector<Solution> feasibleSolutions(0);
    vector<Solution> source_population;

    for (auto p : population)
        if (p.is_feasible)
            feasibleSolutions.push_back(p);

    for (uint parent_idx = 0; parent_idx < parents.size(); parent_idx++)
    {
        if (this->stop()) return;
        if (this->t_t[0] < this->t_target && feasibleSolutions.size() > 1 && parent_idx % 2 == 1 && parents[parent_idx - 1].is_feasible)
            source_population = feasibleSolutions;
        else
            source_population = population;

        for (uint tournament_idx = 0; tournament_idx < tournament_size; tournament_idx++)
        {
            std::uniform_int_distribution<uint> rnd_idx(0, source_population.size() - 1);
            tournament[tournament_idx] = source_population[rnd_idx(*this->rng)];
        }
        sort_by_pf(tournament);
        parents[parent_idx] = tournament[0];
    }
}

//**********************************************************************************************************************
// CROSSOVER
//**********************************************************************************************************************

void EA::insertNode(std::vector<Solution> parents, std::vector<Solution> &children, uint x)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop()) return;
        vector<uint> perm1 = parents[idx].permutation;
        vector<uint> perm2 = parents[idx + 1].permutation;
        vector<uint> new_perm;
        vector<uint> removed_nodes;
        std::uniform_int_distribution<uint> uni(0, this->instance->node_cnt - 1);

        // choosing nodes to be removed from perm1 and perm2
        for (int i = 0; i < (this->instance->node_cnt*x)/100; i++)
            removed_nodes.push_back(uni(*this->rng));
        
        crossover::insertNode(perm1, perm2, new_perm, removed_nodes);
        children[idx] = Solution(new_perm, *this->instance);

        crossover::insertNode(perm2, perm1, new_perm, removed_nodes);
        children[idx+1] = Solution(new_perm, *this->instance);
    }
}

void EA::ERX(std::vector<Solution> parents, std::vector<Solution> &children)
{
     for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop()) return;
        vector<uint> perm1 = parents[idx].permutation;
        vector<uint> perm2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::ERX(perm1, perm2, new_perm, this->instance->lbs,  this->instance->ubs, this->rng);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::ERX(perm2, perm1, new_perm, this->instance->lbs,  this->instance->ubs, this->rng);
        children[idx+1] = Solution(new_perm, *this->instance);
    }
}

//**********************************************************************************************************************
// MUTATION
//**********************************************************************************************************************

void EA::mutation(std::vector<Solution> &children){
    std::uniform_int_distribution<uint> randMutation(0, this->mutationList.size() - 1);
    for (auto &child : children){
        if (this->stop()) return;
        this->mutation_map.at(this->mutationList[randMutation(*this->rng)])(child);
    }
}

void EA::insert(Solution &child)
{
    //LOG(__func__);
    vector<uint> possibleNodes(0);
    for (uint i = 0; i < this->instance->node_cnt; i++)
        if (child.frequency[i] < this->instance->ubs[i]) possibleNodes.push_back(i);
    
    if (possibleNodes.size() == 0) return;

    std::uniform_int_distribution<uint> randNodeIdx(0, possibleNodes.size() - 1);
    std::uniform_int_distribution<uint> randPosition(0, child.permutation.size());

    oprtr::insert(child.permutation, child.frequency, this->instance->ubs, possibleNodes[randNodeIdx(*this->rng)], randPosition(*this->rng));
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::append(Solution &child){
    vector<uint> possibleNodes(0);
    for (uint i = 0; i < this->instance->node_cnt; i++)
        if (child.frequency[i] < this->instance->ubs[i]) possibleNodes.push_back(i);
    
    if (possibleNodes.size() == 0) return;

    std::uniform_int_distribution<uint> randNodeIdx(0, possibleNodes.size() - 1);
    oprtr::append(child.permutation, child.frequency, this->instance->ubs, possibleNodes[randNodeIdx(*this->rng)]);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::remove(Solution &child)
{
    vector<uint> possiblePositions(0);
    for (uint i = 0; i < child.permutation.size(); i++){
        uint node = child.permutation[i];
        if (child.frequency[node] > this->instance->lbs[node]) possiblePositions.push_back(i);
    }
    if (possiblePositions.size() == 0) return;

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
    do {
        nodeX = randomNode(*this->rng);
        nodeY = randomNode(*this->rng);
    } while (nodeX != nodeY );

    oprtr::exchangeIds(child.permutation, child.frequency, this->instance->lbs, this->instance->ubs, nodeX, nodeY);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void EA::twoOpt(Solution &child)
{
    std::uniform_int_distribution<uint> randPosition(0, child.permutation.size() - 1);
    uint posA, posB;
    do {
        posA = randPosition(*this->rng);
        posB = randPosition(*this->rng);
    } while (posA != posB);

    oprtr::reverse(child.permutation, std::min(posA, posB), std::abs((int)posA-(int)posB));
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

//**********************************************************************************************************************
// REPLACEMENT
//**********************************************************************************************************************

template <typename StringType>
size_t levenshtein_distance(const StringType& s1, const StringType& s2) {
    const size_t m = s1.size();
    const size_t n = s2.size();
    if (m == 0)
        return n;
    if (n == 0)
        return m;
    std::vector<size_t> costs(n + 1);
    std::iota(costs.begin(), costs.end(), 0);
    size_t i = 0;
    for (auto c1 : s1) {
        costs[0] = i + 1;
        size_t corner = i;
        size_t j = 0;
        for (auto c2 : s2) {
            size_t upper = costs[j + 1];
            costs[j + 1] = (c1 == c2) ? corner
                : 1 + std::min(std::min(upper, corner), costs[j]);
            corner = upper;
            ++j;
        }
        ++i;
    }
    return costs[n];
}

void EA::nichePopulation(vector<Solution> population, uint capacity, vector<uint> &leaderIdxs, vector<uint> &followerIdxs)
{ 
    for (uint i = 0; i < population.size(); i++){
        if (std::find(followerIdxs.begin(), followerIdxs.end(), i) == followerIdxs.end()){
            leaderIdxs.push_back(i);
            uint occupancy = 1;
            for (uint j = i+1; j < population.size(); j++){
                if (std::find(followerIdxs.begin(), followerIdxs.end(), j) == followerIdxs.end() &&
                        levenshtein_distance(population[i].permutation, population[j].permutation) <= this->nicheRadius){
                    if (occupancy < capacity){
                        leaderIdxs.push_back(i);
                        occupancy += 1;
                    } else {
                        followerIdxs.push_back(j);
                    }
                }
            }
        }
    }
    if (leaderIdxs.size() > 0.4*this->population_size)
        this->nicheRadius = this->nicheRadius > 5 ? this->nicheRadius*1.1 : this->nicheRadius + 1;
    else if (followerIdxs.size() > 0.4*this->population_size)
        this->nicheRadius = this->nicheRadius > 5 ? this->nicheRadius/1.1 : this->nicheRadius - 1;
    
}


void EA::nicheSegregational(std::vector<Solution> children)
{
    if(this->stop()) return;

    copy(this->population.begin(), this->population.end(), std::back_inserter(children));
    sort_by_pf(children);
    vector<uint> feasibleIdxs(0), restIdxs(0), leaderIdxs(0), followerIdxs(0);
    
    this->nichePopulation(children, 1, leaderIdxs, followerIdxs);

    for (uint i = 0; i < children.size(); i++){
        if (children[i].is_feasible)
            feasibleIdxs.push_back(i);
        else 
            restIdxs.push_back(i);
    }

    this->population.clear();

    for (auto idx : feasibleIdxs)
    {
        if (this->population.size() < this->population_size * t_select){
            if (find(this->population.begin(), this->population.end(), children[idx]) == this->population.end())
                this->population.push_back(children[idx]);
        } else {
            restIdxs.push_back(idx);
        }
    }

    sort(restIdxs.begin(), restIdxs.end());

    if (this->population.size() == 0){
        vector<bool> noPenalty(this->instance->penalty_func_cnt, true);
        for (auto idx : restIdxs)
            for (uint i = 0; i < noPenalty.size(); i++)
                if (noPenalty[i] && children[idx].penalties[i] == 0){
                    this->population.push_back(children[idx]);
                    noPenalty[i] = false;
                }      
    }

    while (this->population.size() < this->population_size)
    {
        for (auto idx : restIdxs)
            if (this->population.size() < this->population_size && find(this->population.begin(), this->population.end(), children[idx]) == this->population.end())
                this->population.push_back(children[idx]);
        
        this->construction();
    }
    this->best_known_solution = this->population[0];
}
 


void EA::segregational(std::vector<Solution> children)
{
    if(this->stop()) return;

    copy(this->population.begin(), this->population.end(), std::back_inserter(children));
    sort_by_pf(children);
    vector<uint> feasibleIdxs(0), restIdxs(0);
    
    for (uint i = 0; i < children.size(); i++){
        if (children[i].is_feasible)
            feasibleIdxs.push_back(i);
        else 
            restIdxs.push_back(i);
    }

    this->population.clear();

    for (auto idx : feasibleIdxs)
    {
        if (this->population.size() < this->population_size * t_select){
            if (find(this->population.begin(), this->population.end(), children[idx]) == this->population.end())
                this->population.push_back(children[idx]);
        } else {
            restIdxs.push_back(idx);
        }
    }

    sort(restIdxs.begin(), restIdxs.end());

    // if solution exist with some part of the penalty equal to zero, it wont disappear from the population 
    if (this->population.size() == 0){
        vector<bool> noPenalty(this->instance->penalty_func_cnt, true);
        for (auto idx : restIdxs)
            for (uint i = 0; i < noPenalty.size(); i++)
                if (noPenalty[i] && children[idx].penalties[i] == 0){
                    this->population.push_back(children[idx]);
                    noPenalty[i] = false;
                }      
    }

    while (this->population.size() < this->population_size)
    {
        for (auto idx : restIdxs)
            if (this->population.size() < this->population_size && find(this->population.begin(), this->population.end(), children[idx]) == this->population.end())
                this->population.push_back(children[idx]);
        
        this->construction();
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
    if (constr == "insertNode_5")
        this->crossover = [this](std::vector<Solution> parents, std::vector<Solution> &children)
        { return this->insertNode(parents, children, 5); };
    else if (constr == "insertNode_10")
        this->crossover = [this](std::vector<Solution> parents, std::vector<Solution> &children)
        { return this->insertNode(parents, children, 10); };
    else if (constr == "insertNode_20")
        this->crossover = [this](std::vector<Solution> parents, std::vector<Solution> &children)
        { return this->insertNode(parents, children, 20); };
    else if (constr == "insertNode_50")
        this->crossover = [this](std::vector<Solution> parents, std::vector<Solution> &children)
        { return this->insertNode(parents, children, 50); };
    else if (constr == "ERX")
        this->crossover = [this](std::vector<Solution> parents, std::vector<Solution> &children)
        { return this->ERX(parents, children); };
    else 
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void EA::setReplacement(const string &constr) 
{
    if (constr == "segregational")
        this->replacement = [this](std::vector<Solution> children)
        { return segregational(children); };
    else if (constr == "nicheSegregational")
        this->replacement = [this](std::vector<Solution> children)
        { return nicheSegregational(children); };
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
        penalty_cnt[0] += (uint)solution.is_feasible;
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
    double coef = 1.1;
    
    for (uint idx = 1; idx < this->instance->penalty_func_cnt; idx++)
        if (this->t_t[idx] > this->t_target)
            this->penalty_coefficients[idx] *= coef;
        else 
            this->penalty_coefficients[idx] /= coef;

    double min = *std::min_element(this->penalty_coefficients.begin(), this->penalty_coefficients.end());
    double max = *std::max_element(this->penalty_coefficients.begin(), this->penalty_coefficients.end());
    for (auto &penalty : this->penalty_coefficients)
        penalty = (penalty + 1 - min)*this->penaltyLimit/max; // linear scaling of penalties to range(1 .. this->penaltyLimit)
    
    
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

void EA::initPenaltyCoefs(){
    
    vector<fitness_t> fitness_stats(this->instance->penalty_func_cnt, 0);
    for (auto p : this->population)
        for (uint i = 0; i < fitness_stats.size(); i++)
            fitness_stats[i] += p.penalties[i];
    
    this->penalty_coefficients.push_back(1.);
    for (uint i = 1; i < fitness_stats.size(); i++)
        if (fitness_stats[i] == 0)
            this->penalty_coefficients.push_back(1);
        else
            this->penalty_coefficients.push_back(100*((double)fitness_stats[0])/fitness_stats[i]);
    this->penaltyLimit = *std::max_element(this->penalty_coefficients.begin(), this->penalty_coefficients.end());
}

bool EA::stop(){
    return this->getRuntime() > this->timeout_s || 
        (config["stop_on_feasible"] && this->best_known_solution.is_feasible);
}


void EA::saveToJson(json& container){

}