#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <array>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <random>
#include "types.hpp"
#include "util.hpp"
#include "instance.hpp"
#include "params.hpp"
#include "objective.hpp"

#define CORRECT true
#define ERROR false
#define UPPER true
#define LOWER false

#define SCHEDULE_ERR 100

// size of the array for finding nth_element -> needs to be >= max number of scenarios
#define ARR_SIZE 512

// iterator + sizeof array + 1
#define ARR_END_IT(X, Y) (X + Y + 1)

using namespace std;

namespace sol
{

class Solution {
    private:
        void throw_message(const bool correct, const string &label, const string &message);
        string time_bounds_details(const string &item, const uint_t start_time, const uint_t bound);
        string intervention_time_bounds_details(const string &item, const uint_t start_time, const uint_t bound);
        string workload_bounds_details(const bool upper, const string &item, const uint_t horizon, const fitness_t_ value, const uint_t bound);
        string exclusion_details(const string &first, const string &second, const uint_t horizon);
        uint_to_uint_t start_times; /* map that holds the solution <intervention, start_time> */
    public:
        ins::Instance *instance;
        std::default_random_engine *engine;
        set<uint_t> unscheduled; /* set that holds interventions not yet scheduled */
        set<uint_t> scheduled; /* set that holds interventions already scheduled */

        /* mean risk intermediate results */
        vector<fitness_t_> mean_cumulative_risks;
        /* quantile intermediate results */
        vector<vector<fitness_t_>> risk_st; /* cumulative risks [t][s] */
        vector<fitness_t_> expected_excesses;
        /* objective functions of the solution */
        fitness_t_ mean_risk;
        fitness_t_ expected_excess;
        fitness_t_ final_objective;

        vector<vector<fitness_t_>> resource_use; /* tracks use of resource in time [resource][time] */

        vector<fitness_t_> resource_underuse; /* sum of under lower bound values for each resource */
        fitness_t_ workload_underuse; /* sum of all workloads under lower bounds */

        vector<fitness_t_> resource_overuse; /* sum of above upper bound values for each resource */
        fitness_t_ workload_overuse; /* sum of all workloads above upper bounds */

        // vector<fitness_t_> resource_usage; /* sum of relative usage for each resource over all time horizons */
        // fitness_t_ workload_usage; /* sum of reserves of all resources over all time horizons */

        vector<uint_t> exclusions_at_time; /* vector of number of unmet exclusions in each time */
        uint_t exclusion_penalty; /* total number of unmet exclusions */

        fitness_t_ total_resource_use;

        fitness_t_ extended_objective; /* objective that takes in account number of unmet exclusions and workload out of bounds */

        uint_t restarts_cnt;
        vector<uint_t> unscheduled_cnt; /* counter of interventions unscheduling */

        Solution() = default;
        Solution(ins::Instance *instance, std::default_random_engine *engine);
        void schedule(uint_t intervention_id, uint_t start_time);
        void unschedule(uint_t intervention_id);
        bool is_scheduled(uint_t intervention_id);
        uint_t get_start_time(uint_t intervention_id);
        uint_t get_unscheduled_count();
        bool has_unscheduled();
        uint_t get_first_unscheduled();
        void save(string output_file_path);

        /* simple validity check from state variables */
        bool is_valid();

        Objective estimate_schedule(uint_t intervention_id, uint_t start_time);
        Objective estimate_unschedule(uint_t intervention_id);

        /* -- methods that update state variables based on (un)scheduled intervention */
        void update_state_on_schedule(uint_t scheduled_intervention, uint_t start_time);
        void update_extended_state_on_schedule(uint scheduled_intervention, uint_t start_time);
        void update_state_on_unschedule(uint_t unscheduled_intervention, uint_t start_time);
        void update_extended_state_on_unschedule(uint unscheduled_intervention, uint_t start_time);

        /* -- methods that modify Objective object that holds state of solution would occur if intervention was (un)scheduled -- */
        void estimate_state_on_schedule(uint_t scheduled_intervention, uint_t start_time, Objective *objective);
        void estimate_extended_state_on_schedule(uint scheduled_intervention, uint_t start_time, Objective *objective);
        void estimate_state_on_unschedule(uint_t unscheduled_intervention, uint_t start_time, Objective *objective);
        void estimate_extended_state_on_unschedule(uint unscheduled_intervention, uint_t start_time, Objective *objective);

        /* -- help and print methods -- */
        void print(); /* prints all currently scheduled interventions and their start times */
        void print_state(); /* prints state variables */
        void print_objective(); /* prints only final objective */
        void print_extended_objective(); /* prints only extended objective */
        bool check(); /* runs all checks and solution statistics */
        void info(); /* prints some information about instance */
        void eval(); /* prints evaluation of the solution */
        void print_unscheduled_cnt();

        /* -- checks -- */
        /* check validity of solution */
        bool undefined_intervention();
        bool unscheduled_intervention();
        /* starting date format check is not necessary */
        /* duplicate entry is not necessary */
        bool start_time_out_of_range();
        bool start_time_out_of_intervention_range();
        /* for partial schedules lower bound test does not mean that the schedule is not */
        bool resource_out_of_bounds();
        bool unmet_exclusion();

        /* -- computing state variables -- */
        /* compute statistics of the solution */
        void compute_objectives(); /* computes mean risk and expected excess from scratch (completely new solution) */
        void compute_final_objective(); /* computes final objective function from currently saved values of mean risk and expected excess */
        void compute_workload_misuse(); /* computes overuse and underuse of each resource and total sum */
        void count_unmet_exclusions(); /* computes number of unmet exclusions in each time and total sum */
        void compute_extended_objective(); /* computes extended objective from currently saved values */
        void round_up(int digits); /* rounds up overuse, underuse, updates extended objective */

        /* generates random solution (to test reimplemented checker) */
        void generate_random_solution();
};
}
#endif
