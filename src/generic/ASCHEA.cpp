#include "ASCHEA.hpp"

using nlohmann::json;
using permutator::fitness_t;
using std::pair;
using std::string;
using std::vector;

//**********************************************************************************************************************
// INITIALIZATION
//**********************************************************************************************************************

ASCHEA::ASCHEA(Instance *inst, json config, uint seed)
{
    this->instance = inst;
    this->config = std::move(config);
    this->population_type = this->config["population_type"].get<string>();
    this->population_frequency = this->config["frequency"].get<uint>();
    this->populations[0] = new Population(this->config["population_size"].get<uint>(), this->instance->penalty_func_cnt, this->config["t_target"].get<double>());
    this->active_population = this->populations[0];
    this->best_known_solution = Solution(inst->node_cnt);

    this->timeout_s = this->config["timeout"].get<uint>();
    this->t_select = this->config["t_select"].get<double>();
    this->mutation_rate = this->config["mutation_rate"].get<double>();

    this->setConstruction(this->config["construction"].get<string>());
    this->setSelection(this->config["selection"].get<string>());
    this->setCrossover(this->config["crossover"].get<string>());
    this->mutation_list = this->config["mutation"].get<vector<string>>();
    this->setReplacement(this->config["replacement"].get<string>());
    this->setAlignment(this->config["alignment"].get<string>());

    this->rng = ASCHEA::initRng(seed);
}

void ASCHEA::run()
{
    this->start = std::chrono::steady_clock::now();
    this->construction();
    this->active_population->initializePenalty();

    // uint gap = this->instance->node_cnt;
    // LOG(gap);
    // Solution a = this->active_population->solutions[0];
    // Solution b = this->active_population->solutions[1];

    // alignment::randomUniform(a.permutation, b.permutation, gap, this->rng); //, this->config["diff_p"].get<uint>(), this->config["gap_p"].get<uint>());
    // int width = (int)log10(gap) + 1;
    // for (auto x : a.permutation)
    //     if (x == gap)
    //         std::cout << std::setw(width) << "_" << " ";
    //     else 
    //         std::cout << std::setw(width) << x << " ";
    // std::cout << std::endl;

    // for (auto x : b.permutation)
    //     if (x == gap)
    //         std::cout << std::setw(width) << "_" << " ";
    //     else 
    //         std::cout << std::setw(width) << x << " ";
    // std::cout << std::endl;

    // exit(0);

    uint gen = 0;
    while (!this->stop())
    {
        this->swapPopulation();
        if (this->active_population->generation % 1 == 0)
            std::cout << "pop: " << std::setw(4) << this->active_population->getMaxSize() << " gen: " << std::setw(4) << this->active_population->generation << std::endl;
        vector<Solution> parents(this->active_population->getMaxSize());
        vector<Solution> children(this->active_population->getMaxSize());

        this->constrainedTournament(parents);
        this->crossover(parents, children);
        this->mutation(children);
        this->replacement(children);
        this->active_population->update();

        // not penalized fitness because different populations have different penalties
        if (this->active_population->best_known_solution.fitness < this->best_known_solution.fitness)
        {
            this->best_known_solution = this->active_population->best_known_solution;
            this->best_population = this->active_population;
        }
        gen++;
    }

    // Population* bestPop = this->populations[0];
    // for (auto pop : this->populations){
    //     if (pop != nullptr &&  pop->best_known_solution.fitness < bestPop->best_known_solution.fitness ){
    //         bestPop = pop;
    //     }
    // }
    // this->active_population = bestPop;
    this->best_population->print();
    this->best_known_solution.print();
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
void ASCHEA::constructRandom()
{
    while (this->active_population->getRealSize() < this->active_population->getMaxSize() && !this->stop())
    {
        Solution solution(this->instance->node_cnt);
        construct::random(solution.permutation, solution.frequency, this->instance->lbs, this->rng);
        solution.is_feasible = this->instance->computeFitness(solution.permutation, solution.fitness, solution.penalties);
        this->active_population->append(solution);
    }
}

/*
 * Creates a random permutation of nodes.
 * Replicates the permutation, until the solution is not valid w.r.t. LBs and UBs.
 * Uses this->initial_solution and sets this->current solution.
 */
void ASCHEA::constructRandomReplicate()
{
    while (this->active_population->getRealSize() < this->active_population->getMaxSize() && !this->stop())
    {
        Solution solution(this->instance->node_cnt);
        construct::randomReplicate(solution.permutation, solution.frequency, this->instance->lbs, this->instance->ubs, this->rng);
        solution.is_feasible = this->instance->computeFitness(solution.permutation, solution.fitness, solution.penalties);
        this->active_population->append(solution);
    }
}

void ASCHEA::constructGreedy()
{
    while (this->active_population->getRealSize() < this->active_population->getMaxSize() && !this->stop())
    {
        // random starting point solution
        Solution solution(this->instance->node_cnt);
        for (uint i = 0; i < this->instance->node_cnt / 5; i++)
            this->append(solution);

        bool valid = this->instance->computeFitness(solution.permutation, solution.fitness, solution.penalties);

        while (!valid && !this->stop())
        {
            Solution best_solution = solution;
            fitness_t best_LB_penalty = this->instance->getLBPenalty(best_solution.frequency);
            fitness_t new_fitness;
            vector<fitness_t> new_penalties;
            vector<uint> new_perm = solution.permutation;
            vector<uint> new_freq = solution.frequency;

            for (uint position = 0; position <= solution.permutation.size(); position++)
            {
                for (uint node = 0; node < this->instance->node_cnt; node++)
                {
                    oprtr::insert(new_perm, new_freq, this->instance->ubs, node, position);
                    this->instance->computeFitness(new_perm, new_fitness, new_penalties);
                    fitness_t LB_penalty = this->instance->getLBPenalty(new_freq);
                    if (LB_penalty < best_LB_penalty || (LB_penalty == best_LB_penalty && new_fitness < best_solution.fitness))
                    {
                        best_solution = Solution(new_perm, *this->instance);
                        best_LB_penalty = this->instance->getLBPenalty(best_solution.frequency);
                    }

                    oprtr::remove(new_perm, new_freq, this->instance->lbs, position);
                }
            }
            solution = best_solution;
            valid = this->instance->FrequencyInBounds(solution.frequency);
        }
        solution.is_feasible = this->instance->computeFitness(solution.permutation, solution.fitness, solution.penalties);
        this->active_population->append(solution);
    }
}

void ASCHEA::constructNearestNeighbor()
{
    // // init of penalty coefs (if they dont exist) based on one random solution
    // if (this->population->get_penalty_coefficients().size() == 0 ){
    //     Solution solution(this->instance->node_cnt);
    //     construct::randomReplicate(solution.permutation, solution.frequency, this->instance->lbs, this->instance->ubs, this->rng);
    //     solution.is_feasible = this->instance->computeFitness(solution.permutation, solution.fitness, solution.penalties);
    //     this->population->append(solution);
    //     this->population->initPenaltyCoefs();
    //     this->population->clear();
    // }

    while (this->active_population->getRealSize() < this->active_population->getMaxSize() && !this->stop())
    {
        // random starting point solution
        Solution solution(this->instance->node_cnt);
        for (uint i = 0; i < 4; i++)
            this->append(solution);

        bool valid = this->instance->computeFitness(solution.permutation, solution.fitness, solution.penalties);

        while (!valid && !this->stop())
        {
            Solution best_solution = solution;
            fitness_t best_LB_penalty = this->instance->getLBPenalty(best_solution.frequency);
            fitness_t new_fitness;
            vector<fitness_t> new_penalties;
            vector<uint> new_perm = solution.permutation;
            vector<uint> new_freq = solution.frequency;

            for (uint node = 0; node < this->instance->node_cnt; node++)
            {
                oprtr::append(new_perm, new_freq, this->instance->ubs, node);
                this->instance->computeFitness(new_perm, new_fitness, new_penalties);
                fitness_t LB_penalty = this->instance->getLBPenalty(new_freq);
                if (LB_penalty < best_LB_penalty || (LB_penalty == best_LB_penalty && new_fitness < best_solution.fitness))
                {
                    best_solution = Solution(new_perm, *this->instance);
                    best_LB_penalty = this->instance->getLBPenalty(best_solution.frequency);
                }

                oprtr::remove(new_perm, new_freq, this->instance->lbs, new_perm.size() - 1);
            }
            solution = best_solution;
            valid = this->instance->FrequencyInBounds(solution.frequency);
        }
        solution.is_feasible = this->instance->computeFitness(solution.permutation, solution.fitness, solution.penalties);
        this->active_population->append(solution);
    }
}

//**********************************************************************************************************************
// SELECTION
//**********************************************************************************************************************

void ASCHEA::constrainedTournament(vector<Solution> &parents)
{
    sortByPenalizedFitness(this->active_population->solutions);
    this->constrainedSortedTournament(parents);
}

void ASCHEA::constrainedSortedTournament(vector<Solution> &parents)
{

    uint tournament_size = 4;
    vector<uint> tournament(tournament_size);
    vector<Solution> feasible_solutions(0);
    vector<Solution> source_population;

    for (auto p : this->active_population->solutions)
        if (p.is_feasible)
            feasible_solutions.push_back(p);

    for (uint parent_idx = 0; parent_idx < parents.size(); parent_idx++)
    {
        if (this->stop())
            return;
        if (this->active_population->get_t_t()[0] < this->active_population->get_t_target() && feasible_solutions.size() > 1 && parent_idx % 2 == 1 && parents[parent_idx - 1].is_feasible)
            source_population = feasible_solutions;
        else
            source_population = this->active_population->solutions;

        std::uniform_int_distribution<uint> rnd_idx(0, source_population.size() - 1);
        for (uint tournament_idx = 0; tournament_idx < tournament_size; tournament_idx++)
            tournament[tournament_idx] = rnd_idx(*this->rng);

        parents[parent_idx] = this->active_population->solutions[*min_element(tournament.begin(), tournament.end())];
    }
}

//**********************************************************************************************************************
// CROSSOVER
//**********************************************************************************************************************

void ASCHEA::ERX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::ERX(permutation_1, permutation_2, new_perm, this->instance->lbs, this->instance->ubs, this->rng);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::ERX(permutation_2, permutation_1, new_perm, this->instance->lbs, this->instance->ubs, this->rng);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::AEX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::AEX(permutation_1, permutation_2, new_perm, this->instance->lbs, this->instance->ubs, this->rng);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::AEX(permutation_2, permutation_1, new_perm, this->instance->lbs, this->instance->ubs, this->rng);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::NBX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm;
        
        crossover::NBX(permutation_1, permutation_2, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx] = Solution(new_perm, *this->instance);

        crossover::NBX(permutation_2, permutation_1, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::PBX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::PBX(permutation_1, permutation_2, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::PBX(permutation_2, permutation_1, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx + 1] = Solution(new_perm, *this->instance);  
    }
}

void ASCHEA::OX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::OX(permutation_1, permutation_2, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::OX(permutation_2, permutation_1, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx + 1] = Solution(new_perm, *this->instance);  
    }
}

void ASCHEA::OBX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> frequency_1 = parents[idx].frequency;
        vector<uint> frequency_2 = parents[idx + 1].frequency;
        vector<uint> new_perm(0);

        crossover::OBX(permutation_1, permutation_2, new_perm, frequency_1, frequency_2, this->rng);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::OBX(permutation_2, permutation_1, new_perm, frequency_2, frequency_1, this->rng);
        children[idx + 1] = Solution(new_perm, *this->instance);  
    }
}

void ASCHEA::CX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::CX(permutation_1, permutation_2, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::CX(permutation_2, permutation_1, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::HGreX(vector<Solution> parents, vector<Solution> &children)
{
    std::function<fitness_t(vector<uint>)> f = [this](vector<uint> p)
    {
        return this->getFitness(p);
    };

    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::HGreX(permutation_1, permutation_2, new_perm, this->instance->lbs, this->instance->ubs, f, this->rng);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::HGreX(permutation_2, permutation_1, new_perm, this->instance->lbs, this->instance->ubs, f, this->rng);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::HRndX(vector<Solution> parents, vector<Solution> &children)
{
    std::function<fitness_t(vector<uint>)> f = [this](vector<uint> p)
    {
        return this->getFitness(p);
    };

    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::HRndX(permutation_1, permutation_2, new_perm, this->instance->lbs, this->instance->ubs, f, this->rng);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::HRndX(permutation_2, permutation_1, new_perm, this->instance->lbs, this->instance->ubs, f, this->rng);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::HProX(vector<Solution> parents, vector<Solution> &children)
{
    std::function<fitness_t(vector<uint>)> f = [this](vector<uint> p)
    {
        return this->getFitness(p);
    };

    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::HProX(permutation_1, permutation_2, new_perm, this->instance->lbs, this->instance->ubs, f, this->rng);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::HProX(permutation_2, permutation_1, new_perm, this->instance->lbs, this->instance->ubs, f, this->rng);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::ULX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::rULX(permutation_1, permutation_2, new_perm, this->instance->lbs, this->instance->ubs, false, this->rng, this->alignment);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::rULX(permutation_2, permutation_1, new_perm, this->instance->lbs, this->instance->ubs, false, this->rng, this->alignment);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::RULX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::rULX(permutation_1, permutation_2, new_perm, this->instance->lbs, this->instance->ubs, true, this->rng, this->alignment);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::rULX(permutation_2, permutation_1, new_perm, this->instance->lbs, this->instance->ubs, true, this->rng, this->alignment);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::EULX(vector<Solution> parents, vector<Solution> &children)
{
    std::function<fitness_t(vector<uint>)> f = [this](vector<uint> p)
    {
        return this->getFitness(p);
    };

    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::rULX(permutation_1, permutation_2, new_perm, this->instance->lbs, this->instance->ubs, false, this->rng, this->alignment);
        crossover::EULX(permutation_1, permutation_2, new_perm, f);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::rULX(permutation_2, permutation_1, new_perm, this->instance->lbs, this->instance->ubs, false, this->rng, this->alignment);
        crossover::EULX(permutation_2, permutation_1, new_perm, f);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::ERULX(vector<Solution> parents, vector<Solution> &children)
{
    std::function<fitness_t(vector<uint>)> f = [this](vector<uint> p)
    {
        return this->getFitness(p);
    };

    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::rULX(permutation_1, permutation_2, new_perm, this->instance->lbs, this->instance->ubs, true, this->rng, this->alignment);
        crossover::EULX(permutation_1, permutation_2, new_perm, f);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::rULX(permutation_2, permutation_1, new_perm, this->instance->lbs, this->instance->ubs, true, this->rng, this->alignment);
        crossover::EULX(permutation_2, permutation_1, new_perm, f);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::UPMX(vector<Solution> parents, vector<Solution> &children)
{
    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 = parents[idx].permutation;
        vector<uint> permutation_2 = parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::UPMX(permutation_1, permutation_2, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::UPMX(permutation_2, permutation_1, new_perm, this->instance->node_cnt, this->rng, this->alignment);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::SPX(vector<Solution> parents, vector<Solution> &children)
{
    std::function<fitness_t(vector<uint>)> f = [this](vector<uint> p)
    {
        return this->getFitness(p);
    };

    for (uint idx = 0; idx < children.size(); idx += 2)
    {
        if (this->stop())
            return;
        vector<uint> permutation_1 =  parents[idx].permutation; 
        vector<uint> permutation_2 =  parents[idx + 1].permutation;
        vector<uint> new_perm(0);

        crossover::SPX(permutation_1, permutation_2, new_perm, this->instance->node_cnt, f, this->rng, this->alignment);
        children[idx] = Solution(new_perm, *this->instance);

        new_perm = vector<uint>(0);
        crossover::SPX(permutation_2, permutation_1, new_perm, this->instance->node_cnt, f, this->rng, this->alignment);
        children[idx + 1] = Solution(new_perm, *this->instance);
    }
}

void ASCHEA::PMX(vector<Solution> parents, vector<Solution> &children)
{
    //     for (uint idx = 0; idx < children.size(); idx += 2)
    //     {
    //         if (this->stop())
    //             return;
    //         vector<uint> permutation_1 = parents[idx].permutation;
    //         vector<uint> permutation_2 = parents[idx + 1].permutation;
    //         vector<uint> new_perm(0);

    //         LOG("--------------");
    //         parents[idx].print();
    //         parents[idx+1].print();

    //         // crossover::PMX(permutation_1, permutation_2, new_perm, this->instance->node_cnt, this->rng);
    //         children[idx] = Solution(new_perm, *this->instance);
    //         children[idx].print();

    //         new_perm = vector<uint>(0);
    //         // crossover::PMX(permutation_2, permutation_1, new_perm, this->instance->node_cnt, this->rng);
    //         children[idx + 1] = Solution(new_perm, *this->instance);
    //         children[idx+1].print();
    //         exit(0);
    //     }
}

//**********************************************************************************************************************
// MUTATION
//**********************************************************************************************************************

void ASCHEA::mutation(vector<Solution> &children)
{
    std::uniform_int_distribution<uint> mutation_rng(0, this->mutation_list.size() - 1);
    std::uniform_real_distribution<> norm_rng(0.0, 1.0);
    for (auto &child : children)
    {
        if (this->stop())
            return;
        if (norm_rng(*this->rng) < this->mutation_rate)
            this->mutation_map.at(this->mutation_list[mutation_rng(*this->rng)])(child);
    }
}

void ASCHEA::insert(Solution &child)
{
    vector<uint> possible_nodes(0);
    for (uint i = 0; i < this->instance->node_cnt; i++)
        if (child.frequency[i] < this->instance->ubs[i])
            possible_nodes.push_back(i);

    if (possible_nodes.size() == 0)
        return;

    std::uniform_int_distribution<uint> node_rng(0, possible_nodes.size() - 1);
    std::uniform_int_distribution<uint> position_rng(0, child.permutation.size());

    oprtr::insert(child.permutation, child.frequency, this->instance->ubs, possible_nodes[node_rng(*this->rng)], position_rng(*this->rng));
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void ASCHEA::append(Solution &child)
{
    vector<uint> possible_nodes(0);
    for (uint i = 0; i < this->instance->node_cnt; i++)
        if (child.frequency[i] < this->instance->ubs[i])
            possible_nodes.push_back(i);

    if (possible_nodes.size() == 0)
        return;

    std::uniform_int_distribution<uint> node_rng(0, possible_nodes.size() - 1);
    oprtr::append(child.permutation, child.frequency, this->instance->ubs, possible_nodes[node_rng(*this->rng)]);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void ASCHEA::remove(Solution &child)
{
    vector<uint> possible_positions(0);
    for (uint i = 0; i < child.permutation.size(); i++)
    {
        uint node = child.permutation[i];
        if (child.frequency[node] > this->instance->lbs[node])
            possible_positions.push_back(i);
    }
    if (possible_positions.size() == 0)
        return;

    std::uniform_int_distribution<uint> position_rng(0, possible_positions.size() - 1);
    oprtr::remove(child.permutation, child.frequency, this->instance->lbs, possible_positions[position_rng(*this->rng)]);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void ASCHEA::centeredExchange(Solution &child, uint x)
{
    std::uniform_int_distribution<uint> position_rng(x, child.permutation.size() - x - 1);
    oprtr::centeredExchange(child.permutation, position_rng(*this->rng), x);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void ASCHEA::relocate(Solution &child, uint x, bool reverse)
{
    std::uniform_int_distribution<uint> position_rng(0, child.permutation.size() - x);
    oprtr::relocate(child.permutation, position_rng(*this->rng), position_rng(*this->rng), x, reverse);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void ASCHEA::exchange(Solution &child, uint x, uint y, bool reverse)
{
    std::uniform_int_distribution<uint> position_X_rng(0, child.permutation.size() - x);
    std::uniform_int_distribution<uint> position_Y_rng(0, child.permutation.size() - y);

    oprtr::exchange(child.permutation, position_X_rng(*this->rng), position_Y_rng(*this->rng), x, y, reverse);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}
void ASCHEA::moveAll(Solution &child, uint x)
{
    std::uniform_int_distribution<uint> node_rng(0, this->instance->node_cnt - 1);
    std::uniform_int_distribution<int> offset_rng(-x, x);
    oprtr::moveAll(child.permutation, node_rng(*this->rng), offset_rng(*this->rng));
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void ASCHEA::exchangeIds(Solution &child)
{
    std::uniform_int_distribution<uint> node_rng(0, this->instance->node_cnt - 1);
    uint nodeX, nodeY;
    do
    {
        nodeX = node_rng(*this->rng);
        nodeY = node_rng(*this->rng);
    } while (nodeX != nodeY);

    oprtr::exchangeIds(child.permutation, child.frequency, this->instance->lbs, this->instance->ubs, nodeX, nodeY);
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

void ASCHEA::twoOpt(Solution &child)
{
    std::uniform_int_distribution<uint> position_rng(0, child.permutation.size() - 1);
    uint posA, posB;
    do
    {
        posA = position_rng(*this->rng);
        posB = position_rng(*this->rng);
    } while (posA != posB);

    oprtr::reverse(child.permutation, std::min(posA, posB), std::abs((int)posA - (int)posB));
    child.is_feasible = this->instance->computeFitness(child.permutation, child.fitness, child.penalties);
}

//**********************************************************************************************************************
// REPLACEMENT
//**********************************************************************************************************************

void ASCHEA::nicheSegregational(vector<Solution> children)
{
    if (this->stop())
        return;

    copy(this->active_population->solutions.begin(), this->active_population->solutions.end(), std::back_inserter(children));
    sortByPenalizedFitness(children);

    vector<bool> leader_mask(0), follower_mask(0);
    this->nichePopulation(children, 1, leader_mask, follower_mask);
    int leader_follower_balance = 0;

    vector<uint> feasible_idxs(0), rest_of_idxs(0);

    for (uint i = 0; i < children.size(); i++)
    {
        if (children[i].is_feasible)
            feasible_idxs.push_back(i);
        else
            rest_of_idxs.push_back(i);
    }

    this->active_population->clear();

    for (auto mask : {leader_mask, follower_mask})
    {
        for (auto idx : feasible_idxs)
        {
            if (!mask[idx])
                continue;
            if (this->active_population->getRealSize() < this->active_population->getMaxSize() * t_select)
            {
                if (true || find(this->active_population->solutions.begin(), this->active_population->solutions.end(), children[idx]) == this->active_population->solutions.end())
                {
                    this->active_population->append(children[idx]);
                    leader_follower_balance += leader_mask[idx] ? 1 : -1;
                }
            }
            else
            {
                rest_of_idxs.push_back(idx);
            }
        }
    }

    sort(rest_of_idxs.begin(), rest_of_idxs.end());

    for (auto mask : {leader_mask, follower_mask})
    {
        for (auto idx : rest_of_idxs)
            if (mask[idx] && this->active_population->getRealSize() < this->active_population->getMaxSize() && (true || find(this->active_population->solutions.begin(), this->active_population->solutions.end(), children[idx]) == this->active_population->solutions.end()))
            {
                this->active_population->append(children[idx]);
                leader_follower_balance += leader_mask[idx] ? 1 : -1;
            }
    }
    this->construction();

    // adaptive niche radius
    if (leader_follower_balance > 0)
        this->active_population->niche_radius += 1;
    else if (leader_follower_balance < 0)
        this->active_population->niche_radius -= 1;
}

void ASCHEA::segregational(vector<Solution> children)
{
    if (this->stop())
        return;

    copy(this->active_population->solutions.begin(), this->active_population->solutions.end(), std::back_inserter(children));
    sortByPenalizedFitness(children);
    vector<uint> feasible_idxs(0), rest_of_idxs(0);

    for (uint i = 0; i < children.size(); i++)
    {
        if (children[i].is_feasible)
            feasible_idxs.push_back(i);
        else
            rest_of_idxs.push_back(i);
    }

    this->active_population->clear();

    for (auto idx : feasible_idxs)
    {
        if (this->active_population->getRealSize() < this->active_population->getMaxSize() * t_select)
        {
            if (find(this->active_population->solutions.begin(), this->active_population->solutions.end(), children[idx]) == this->active_population->solutions.end())
                this->active_population->append(children[idx]);
        }
        else
        {
            rest_of_idxs.push_back(idx);
        }
    }

    sort(rest_of_idxs.begin(), rest_of_idxs.end());

    // if solution exist with some part of the penalty equal to zero, it wont disappear from the population
    // if size > 0 then feasible solution exist and its penalties all equal 0
    if (this->active_population->getRealSize() == 0)
    {
        vector<bool> zero_penalty_presence(this->instance->penalty_func_cnt, true);
        for (auto idx : rest_of_idxs)
        {
            bool add = false;
            for (uint i = 0; i < zero_penalty_presence.size(); i++)
                if (zero_penalty_presence[i] && children[idx].penalties[i] == 0)
                {
                    zero_penalty_presence[i] = false;
                    add = true;
                }
            if (add)
                this->active_population->append(children[idx]);
        }
    }

    for (auto idx : rest_of_idxs)
        if (this->active_population->getRealSize() < this->active_population->getMaxSize() && find(this->active_population->solutions.begin(), this->active_population->solutions.end(), children[idx]) == this->active_population->solutions.end())
            this->active_population->append(children[idx]);

    this->construction();
}

//**********************************************************************************************************************
// SET OPERATIONS
//**********************************************************************************************************************

void ASCHEA::setConstruction(const string &constr)
{
    if (constr == "random")
        this->construction = [this]
        { return this->constructRandom(); };
    else if (constr == "randomReplicate")
        this->construction = [this]
        { return this->constructRandomReplicate(); };
    else if (constr == "greedy")
        this->construction = [this]
        { return this->constructGreedy(); };
    else if (constr == "nearestNeighbor")
        this->construction = [this]
        { return this->constructNearestNeighbor(); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void ASCHEA::setSelection(const string &constr)
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

void ASCHEA::setCrossover(const string &constr)
{
    if (constr == "NBX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->NBX(parents, children); };
    else if (constr == "PBX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->PBX(parents, children); };
    else if (constr == "ERX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->ERX(parents, children); };
    else if (constr == "AEX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->AEX(parents, children); };
    else if (constr == "OX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->OX(parents, children); };
    else if (constr == "OBX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->OBX(parents, children); };
    else if (constr == "CX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->CX(parents, children); };
    else if (constr == "PMX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->PMX(parents, children); };
    else if (constr == "HGreX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->HGreX(parents, children); };
    else if (constr == "HRndX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->HRndX(parents, children); };
    else if (constr == "HProX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->HProX(parents, children); };
    else if (constr == "ULX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->ULX(parents, children); };
    else if (constr == "RULX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->RULX(parents, children); };
    else if (constr == "EULX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->EULX(parents, children); };
    else if (constr == "ERULX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->ERULX(parents, children); };
    else if (constr == "UPMX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->UPMX(parents, children); };
    else if (constr == "SPX")
        this->crossover = [this](vector<Solution> parents, vector<Solution> &children)
        { return this->SPX(parents, children); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void ASCHEA::setAlignment(const string &constr)
{
    if (constr == "globalUniform")
        this->alignment = [this](vector<uint> &permutation_1, vector<uint> &permutation_2, uint gap_node)
        { return alignment::globalUniform(permutation_1, permutation_2, gap_node, 1, 1); };
    else if (constr == "globalOneGap")
        this->alignment = [this](vector<uint> &permutation_1, vector<uint> &permutation_2, uint gap_node)
        { return alignment::globalOneGap(permutation_1, permutation_2, gap_node); };
    else if (constr == "randomUniform")
        this->alignment = [this](vector<uint> &permutation_1, vector<uint> &permutation_2, uint gap_node)
        { return alignment::randomUniform(permutation_1, permutation_2, gap_node, this->rng); };
    else if (constr == "randomOneGap")
        this->alignment = [this](vector<uint> &permutation_1, vector<uint> &permutation_2, uint gap_node)
        { return alignment::randomOneGap(permutation_1, permutation_2, gap_node, this->rng); };
    else if (constr == "backfillOneGap")
        this->alignment = [this](vector<uint> &permutation_1, vector<uint> &permutation_2, uint gap_node)
        { return alignment::backfillOneGap(permutation_1, permutation_2, gap_node); };
    else if (constr == "frontfillOneGap")
        this->alignment = [this](vector<uint> &permutation_1, vector<uint> &permutation_2, uint gap_node)
        { return alignment::frontfillOneGap(permutation_1, permutation_2, gap_node); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void ASCHEA::setReplacement(const string &constr)
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

fitness_t ASCHEA::getFitness(vector<uint> permutation)
{
    vector<fitness_t> penalties;
    fitness_t fitness;
    this->instance->computeFitness(permutation, fitness, penalties);
    return this->penalizedFitness(penalties);
}

void ASCHEA::nichePopulation(vector<Solution> population, uint capacity, vector<bool> &leader_mask, vector<bool> &follower_mask)
{
    leader_mask = vector<bool>(population.size(), false);
    follower_mask = vector<bool>(population.size(), false);

    for (uint i = 0; i < population.size(); i++)
    {
        if (!follower_mask[i])
        {
            leader_mask[i] = true;
            uint occupancy = 1;
            for (uint j = i + 1; j < population.size(); j++)
            {
                if (!follower_mask[j] && levenshtein_distance(population[i].permutation, population[j].permutation) <= this->active_population->niche_radius)
                {
                    if (occupancy < capacity)
                    {
                        leader_mask[j] = true;
                        occupancy += 1;
                    }
                    else
                    {
                        follower_mask[j] = true;
                    }
                }
            }
        }
    }
}

std::mt19937 *ASCHEA::initRng(uint seed)
{
    if (seed == 0)
    {
        std::random_device rd;
        seed = rd();
    }
    return new std::mt19937(seed);
}

void ASCHEA::sortByPenalizedFitness(vector<Solution> &v)
{
    std::sort(v.begin(), v.end(),
              [this](Solution A, Solution B) -> bool
              {
                  return this->penalizedFitness(A) < this->penalizedFitness(B);
              });
}

fitness_t ASCHEA::penalizedFitness(Solution solution)
{
    return (fitness_t)round(inner_product(solution.penalties.begin(), solution.penalties.end(), this->active_population->get_penalty_coefficients().begin(), 0.));
}

fitness_t ASCHEA::penalizedFitness(vector<fitness_t> penalties)
{
    return (fitness_t)round(inner_product(penalties.begin(), penalties.end(), this->active_population->get_penalty_coefficients().begin(), 0.));
}

bool ASCHEA::stop()
{
    return this->getRuntime() > this->timeout_s ||
           (config["stop_on_feasible"] && this->best_known_solution.is_feasible);
}

void ASCHEA::swapPopulation()
{
    // choose the next population to run
    int active_population_idx = -1;
    do {
        active_population_idx++;
        this->population_counter[active_population_idx] = (this->population_counter[active_population_idx] + 1) % this->population_frequency;
    } while (this->population_counter[active_population_idx] == 0);
    
    if (this->populations[active_population_idx] != nullptr)
    {
        this->active_population = this->populations[active_population_idx];
    }
    else
    {
        this->populations[active_population_idx] = new Population((this->populations[active_population_idx - 1]->getMaxSize()) * 2, this->instance->penalty_func_cnt, this->config["t_target"].get<double>());
        this->active_population = this->populations[active_population_idx];
        this->construction();
        this->active_population->initializePenalty();
    }

    this->active_population->generation++;

    for (uint idx2 = 0; idx2 < active_population_idx; idx2++)
    {
        if (this->active_population->avg_fitness < this->populations[idx2]->avg_fitness || this->populations[idx2]->is_stalled)
        {
            std::cout << "--------erased: " << this->populations[idx2]->getMaxSize() << " stall: " << (this->populations[idx2]->is_stalled ? "true" : "false") << "  | fitness: " << this->active_population->avg_fitness << "|" << this->populations[idx2]->avg_fitness << std::endl;
            if (this->best_population == this->populations[idx2])
                this->best_population = this->active_population;
            delete this->populations[idx2];
            this->populations.erase(this->populations.begin() + idx2);
            this->population_counter.erase(this->population_counter.begin() + idx2);
            active_population_idx -= 1;
        }
    }
}

void ASCHEA::saveToJson(json &container)
{
}
