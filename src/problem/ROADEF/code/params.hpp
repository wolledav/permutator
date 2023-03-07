#ifndef PARAMS_H
#define PARAMS_H

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include "types.hpp"
#include "util.hpp"
#include "instance.hpp"

// extended objective weights
#define BETA_LOWER_LABEL ("BETA_LOWER")
#define BETA_UPPER_LABEL ("BETA_UPPER")
#define GAMMA_LABEL ("GAMMA")
// local search
#define LS_FIRST_IMPROVE_LABEL ("LS_FIRST_IMPROVE")
#define TWO_SHIFT_LIMIT_LABEL ("TWO_SHIFT_LIMIT")
#define ONE_SHIFT_DEPTH_LABEL ("ONE_SHIFT_DEPTH")
// ALNS
#define ALNS_DEPTH_LABEL ("ALNS_DEPTH")
#define OMEGA_1_LABEL ("OMEGA_1")
#define OMEGA_2_LABEL ("OMEGA_2")
#define OMEGA_3_LABEL ("OMEGA_3")
#define OMEGA_4_LABEL ("OMEGA_4")
#define LAMBDA_LABEL ("LAMBDA")
#define NU_LABEL ("NU")
#define MU1_LABEL ("MU1")
#define MU2_LABEL ("MU2")
#define LENGTH_BATCH_LABEL ("LENGTH_BATCH")
#define COST_BATCH_LABEL ("COST_BATCH")
#define RD_BATCH_LABEL ("RD_BATCH")
#define ACCEPT_TOLERANCE_LABEL ("ACCEPT_TOLERANCE")
// greedy_search only
#define ITERS_MAX_LABEL ("ITERS_MAX")
// verbose outputs
#define VERBOSE_CONFIG (false)
#define VERBOSE_ALNS (false)
#define VERBOSE_SOLUTION (false)
#define VERBOSE_LS (false)
#define VERBOSE_CONS (false)
#define VERBOSE_HEURISTICS (false)
#define VERBOSE_SA (true)
// other
#define INPUT_PATH_LABEL ("-p")
#define OUTPUT_PATH_LABEL ("-o")
#define TIME_LIMIT_LABEL ("-t")
#define SEED_LABEL ("-s")
#define TEAM_ID_LABEL ("-name")
#define TEAM_ID 49
#define CONSTRUCTION_LABEL ("CONSTRUCTION")
#define NUMERIC_TOLERANCE 0.00001
#define TIME_RESERVE 600
// Continuous solution saving
#define SAVE_SOL_PROGRESS (false)
#define SOL_PROGRESS_DIR ("./solutions/sol_progress/")
extern uint_t sol_progress_cnt;

extern std::string INPUT_PATH;
extern std::string OUTPUT_PATH;
// extern std::default_random_engine ENGINE;
extern uint_t TIME_LIMIT; // ALNS loop time limit (in milliseconds)
extern std::chrono::steady_clock::time_point BEGIN;
extern bool RETURN_ID;
extern int SEED;

// DEPENDENT PARAMETERS - COEFFICIENTS
extern int ITERS_MAX;
extern double ITERS_MAX_offset_short;
extern vector<double> ITERS_MAX_coefs_short;
extern double ITERS_MAX_offset_long;
extern vector<double> ITERS_MAX_coefs_long;

extern double ALNS_DEPTH; // ratio determining max. depth of destruction (0, 1>
extern double ALNS_DEPTH_offset_short;
extern vector<double> ALNS_DEPTH_coefs_short;
extern double ALNS_DEPTH_offset_long;
extern vector<double> ALNS_DEPTH_coefs_long;

extern double LENGTH_BATCH; // rel. size of subset of interventions considered in length-based insertions
extern double LENGTH_BATCH_offset_short;
extern vector<double> LENGTH_BATCH_coefs_short;
extern double LENGTH_BATCH_offset_long;
extern vector<double> LENGTH_BATCH_coefs_long;

extern double COST_BATCH; // rel. size of subset of interventions considered in cost-based insertions
extern double COST_BATCH_offset_short;
extern vector<double> COST_BATCH_coefs_short;
extern double COST_BATCH_offset_long;
extern vector<double> COST_BATCH_coefs_long;

extern double RD_BATCH; // rel. size of subset of interventions considered in resource usage-based insertions
extern double RD_BATCH_offset_short;
extern vector<double> RD_BATCH_coefs_short;
extern double RD_BATCH_offset_long;
extern vector<double> RD_BATCH_coefs_long;

extern double ACCEPT_TOLERANCE;
extern double ACCEPT_TOLERANCE_offset_short;
extern vector<double> ACCEPT_TOLERANCE_coefs_short;
extern double ACCEPT_TOLERANCE_offset_long;
extern vector<double> ACCEPT_TOLERANCE_coefs_long;
extern vector<double> ACCEPT_TOLERANCE_values;

extern double OMEGA_1; // new solution is better than global best
extern double OMEGA_1_offset_short;
extern vector<double> OMEGA_1_coefs_short;
extern double OMEGA_1_offset_long;
extern vector<double> OMEGA_1_coefs_long;

extern double OMEGA_2; // new solution is better than the current one
extern double OMEGA_2_offset_short;
extern vector<double> OMEGA_2_coefs_short;
extern double OMEGA_2_offset_long;
extern vector<double> OMEGA_2_coefs_long;

extern double OMEGA_4; // new solution is rejected
extern double OMEGA_4_offset_short;
extern vector<double> OMEGA_4_coefs_short;
extern double OMEGA_4_offset_long;
extern vector<double> OMEGA_4_coefs_long;

extern double LAMBDA; // decay parameter <0, 1> for heuristic weights update; 0 - no adjusting, 1 - no memory
extern double LAMBDA_offset_short;
extern vector<double> LAMBDA_coefs_short;
extern double LAMBDA_offset_long;
extern vector<double> LAMBDA_coefs_long;

extern double ONE_SHIFT_DEPTH;
extern double ONE_SHIFT_DEPTH_offset_short;
extern vector<double> ONE_SHIFT_DEPTH_coefs_short;
extern double ONE_SHIFT_DEPTH_offset_long;
extern vector<double> ONE_SHIFT_DEPTH_coefs_long;

extern uint_t TWO_SHIFT_LIMIT;
extern double TWO_SHIFT_LIMIT_offset_short;
extern vector<double> TWO_SHIFT_LIMIT_coefs_short;
extern double TWO_SHIFT_LIMIT_offset_long;
extern vector<double> TWO_SHIFT_LIMIT_coefs_long;

extern bool LS_FIRST_IMPROVE;
extern double LS_FIRST_IMPROVE_offset_short;
extern vector<double> LS_FIRST_IMPROVE_coefs_short;
extern double LS_FIRST_IMPROVE_offset_long;
extern vector<double> LS_FIRST_IMPROVE_coefs_long;

extern uint_t BETA_LOWER;
extern double BETA_LOWER_offset_short;
extern vector<double> BETA_LOWER_coefs_short;
extern double BETA_LOWER_offset_long;
extern vector<double> BETA_LOWER_coefs_long;

extern uint_t BETA_UPPER;
extern double BETA_UPPER_offset_short;
extern vector<double> BETA_UPPER_coefs_short;
extern double BETA_UPPER_offset_long;
extern vector<double> BETA_UPPER_coefs_long;

extern uint_t GAMMA;
extern double GAMMA_offset_short;
extern vector<double> GAMMA_coefs_short;
extern double GAMMA_offset_long;
extern vector<double> GAMMA_coefs_long;

extern double NU; // additive noise in cost evaluation <0, inf)
extern double NU_offset_short;
extern vector<double> NU_coefs_short;
extern double NU_offset_long;
extern vector<double> NU_coefs_long;

// base probability value in hybrid heuristics; interventions are selected according to distribution [mu^0, mu^1...]
extern double MU1; // MU1 is for "almost smallest" heuristics -> MU1 if from (0, 1)
extern double MU1_offset_short;
extern vector<double> MU1_coefs_short;
extern double MU1_offset_long;
extern vector<double> MU1_coefs_long;

extern double MU2; // , MU2 is for "almost largest" -> MU2 is from (1, inf)
extern double MU2_offset_short;
extern vector<double> MU2_coefs_short;
extern double MU2_offset_long;
extern vector<double> MU2_coefs_long;

extern long long INSERTIONS1;
extern double INSERTIONS1_offset_short;
extern vector<double> INSERTIONS1_coefs_short;
extern double INSERTIONS1_offset_long;
extern vector<double> INSERTIONS1_coefs_long;

extern long long INSERTIONS2;
extern double INSERTIONS2_offset_short;
extern vector<double> INSERTIONS2_coefs_short;
extern double INSERTIONS2_offset_long;
extern vector<double> INSERTIONS2_coefs_long;

extern long long REMOVALS;
extern double REMOVALS_offset_short;
extern vector<double> REMOVALS_coefs_short;
extern double REMOVALS_offset_long;
extern vector<double> REMOVALS_coefs_long;

extern long long LS;
extern double LS_offset_short;
extern vector<double> LS_coefs_short;
extern double LS_offset_long;
extern vector<double> LS_coefs_long;

extern long long CONS;
extern double CONS_offset_short;
extern vector<double> CONS_coefs_short;
extern double CONS_offset_long;
extern vector<double> CONS_coefs_long;


// FIXED PARAMETERS
extern double INITIAL_WEIGHT; // heuristic initial weight in ALNS

enum insertion_idx {
    RANDOM_INSERT, FIXED_INSERT, CHEAPEST_INSERT, MOST_EXPENSIVE_INSERT, LRD1_INSERT,
    LRD2_INSERT, HRD_INSERT, LONGEST1_INSERT, LONGEST2_INSERT, SHORTEST1_INSERT,
    SHORTEST2_INSERT, MOST_EXCLUSIONS_INSERT, LEAST_EXCLUSIONS_INSERT, MOST_USED_INSERT, LEAST_USED_INSERT,
    N1_RANDOM_INSERT, N1_FIXED_INSERT, N1_CHEAPEST_INSERT, N1_MOST_EXPENSIVE_INSERT, N1_LRD1_INSERT, N1_HRD_INSERT, N1_LONGEST1_INSERT,
    N1_SHORTEST1_INSERT, N1_MOST_EXCLUSIONS_INSERT, N1_LEAST_EXCLUSIONS_INSERT, N1_MOST_USED_INSERT, N1_LEAST_USED_INSERT,
    N2_CHEAPEST_INSERT, N2_MOST_EXPENSIVE_INSERT, N2_LRD1_INSERT, N2_HRD_INSERT, N2_LONGEST1_INSERT,
    N2_SHORTEST1_INSERT, N2_MOST_EXCLUSIONS_INSERT, N2_LEAST_EXCLUSIONS_INSERT, N2_MOST_USED_INSERT, N2_LEAST_USED_INSERT,
    N3_CHEAPEST_INSERT, N3_MOST_EXPENSIVE_INSERT, N3_LRD1_INSERT, N3_HRD_INSERT, N3_LONGEST1_INSERT,
    N3_SHORTEST1_INSERT, N3_MOST_EXCLUSIONS_INSERT, N3_LEAST_EXCLUSIONS_INSERT, N3_MOST_USED_INSERT, N3_LEAST_USED_INSERT,
    INSERT_COUNT
};

enum removal_idx {
    RANDOM_REMOVE, CHEAPEST_REMOVE, MOST_EXPENSIVE_REMOVE, LRD_REMOVE, HRD_REMOVE,
    LONGEST_REMOVE, SHORTEST_REMOVE, MOST_EXCLUSIONS_REMOVE, LEAST_EXCLUSIONS_REMOVE,
    MOST_USED_REMOVE, LEAST_USED_REMOVE,
    REMOVE_COUNT
};

enum construction_idx {
    RANDOM_CONSTRUCT,
    LONGEST1_CONSTRUCT,
    HRD_CONSTRUCT,
    MOST_EXPENSIVE_CONSTRUCT,
    CHEAPEST_CONSTRUCT, LRD1_CONSTRUCT, LRD2_CONSTRUCT,
    FIXED_CONSTRUCT, LONGEST2_CONSTRUCT, MOST_EXCLUSIONS_CONSTRUCT,
    SHORTEST1_CONSTRUCT, SHORTEST2_CONSTRUCT,
    CONSTRUCTION_COUNT
};

enum ls_idx {
    ONE_SHIFT, EXCL_TWO_SHIFT, RAND_TWO_SHIFT,
    LS_COUNT
};

extern const char* insertions_labels[INSERT_COUNT];
extern const char* removals_labels[REMOVE_COUNT];
extern const char* constructions_labels[CONSTRUCTION_COUNT];
extern const char* ls_labels[LS_COUNT];

int get_insertion_idx(char *label);
int get_removal_idx(char *label);
int get_construct_idx(char *label);
int get_ls_idx(char *label);

void dump_config();

std::string format_output_path(std::string directories, std::string input);

void parse_global_params(int argc, char *argv[]);

void determine_dependent_params(ins::Instance *instance);

bool stop();

class ALNS_setup {
    public:
        ALNS_setup();
        bool default_insertions = true;
        bool default_removals = true;
        bool default_ls = true;
        std::vector<uint_t> insertions;
        std::vector<uint_t> removals;
        std::vector<uint_t> ls;
        uint_t construction;
        void check_setup();
};

extern ALNS_setup methods;

#endif
