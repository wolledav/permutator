#include "ROADEF.hpp"

using permutator::fitness_t;
using std::vector;

ROADEFInstance::ROADEFInstance(const char *path)
{
    this->parseDataFrom(path);
    this->name = path;
    this->node_cnt = this->instance.get_intervention_count() + 1;
    this->penalty_func_cnt = 4;
    this->lbs.resize(node_cnt);
    this->ubs.resize(node_cnt);
    for (uint i = 1; i < node_cnt; i++){
        lbs[i] = 1;
        ubs[i] = 1;
    }
    this->lbs[0] = 0;
    this->ubs[0] = this->instance.get_horizon_num();
}


void ROADEFInstance::parseDataFrom(const char *path)
{
    Parser *parser = new Parser(string(path));
    parser->load();

    parser->process(&this->instance);

    delete parser;
    std::default_random_engine* rng(0);
    this->solution = ROADEF::Solution(&this->instance, rng);
}

void ROADEFInstance::scheduleStartTimes(const std::vector<uint> &permutation)
{
    uint time = 1;
    for (auto p : permutation){
        if (p == 0) {
            time++;
        } else {
            time = min(time, this->instance.get_t_max(p));
            if (this->solution.is_scheduled(p)){
                if (this->solution.get_start_time(p) != time){
                    this->solution.unschedule(p);
                    this->solution.schedule(p, time);
                }
            } else {
                this->solution.schedule(p, time);
            }
        }
    }
}

//#pragma omp critical
bool ROADEFInstance::computeFitness(const std::vector<uint> &permutation, permutator::fitness_t &fitness, std::vector<permutator::fitness_t> &penalties)
{    
    this->scheduleStartTimes(permutation);
    this->solution.compute_objectives();
    this->solution.compute_workload_misuse();
    this->solution.count_unmet_exclusions();

    penalties.clear();
    penalties.push_back((fitness_t)round(this->solution.final_objective));
    penalties.push_back((fitness_t)round(1000*this->solution.workload_underuse)); //*1000 used for .3 decimal precision
    penalties.push_back((fitness_t)round(1000*this->solution.workload_overuse));
    penalties.push_back(this->solution.exclusion_penalty);
    fitness = penalties[0] + WORKLOAD_PENALTY*(penalties[1] + penalties[2]) + EXCLUSION_PENALTY*penalties[3];

    return this->solution.is_valid();
}

void ROADEFInstance::print_nodes(){
    LOG("print nodes");
}

void ROADEFInstance::print_solution(Solution *solution_, std::basic_ostream<char> &outf){
    solution_->print();
}