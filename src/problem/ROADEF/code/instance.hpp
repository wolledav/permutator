#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <map>

#include "hashing.hpp"
#include "util.hpp"
#include "types.hpp"
#include "ranges.hpp"


using namespace std;
using namespace hashing;



typedef unordered_map<string, uint_t> string_to_uint_t;
typedef unordered_map<uint_t, string> uint_to_string_t;
typedef unordered_map<uint_t, uint_t> uint_to_uint_t;
typedef unordered_map<pair_key, uint_t, pair_hash> pair_to_uint_t;
typedef vector<half_fitness_t_> half_fitness_vector_t;
typedef vector<uint_t> uint_vector_t;
typedef set<uint_t> uint_set_t;
typedef pair<uint_t, uint_t> uint_pair_t;
typedef vector<uint_pair_t> uint_pair_vector_t;
typedef vector<vector<uint_t>> uint_vector_2d_t;
typedef vector<uint_set_t> uint_set_vector_t;

namespace ROADEF
{

class Instance {
    private:
        /* variables that hold products of different parameters to avoid unnecessary multiplication */
        uint_t rxt = PROD_RT; // size of array [r][t]
        uint_t ixt = PROD_IT; // size of array [i][t]
        uint_t ixt_ = PROD_IT; // size of array [i][t']
        uint_t ixt_xt = PROD_ITT; // size of array [i][t'][t]
        uint_t ixt_xr = PROD_ITR; // size of array [i][t'][r]
        uint_t ixt_xrxt = PROD_ITRT; // size of array [i][t'][r][t]
        uint_t t_xrxt = PROD_TRT; // size of array [t'][r][t]
        uint_t t_xt = PROD_TT; // size of array [t'][t]

        /* resource maps and counter */
        uint_t resource_count = 1;
        string_to_uint_t resource_to_id;
        uint_to_string_t id_to_resource;
        uint_vector_t resources; /* holds all ids of resources in the maps */

        /* intervnetion maps and count */
        uint_t intervention_count = 1;
        string_to_uint_t intervention_to_id;
        uint_to_string_t id_to_intervention;
        uint_vector_t interventions; /* holds all ids of interventions in the maps */

        /* holds resources that are used by an intervention */
        uint_set_vector_t resources_used_by_intervention;

        /* horizons and scenarios fields */
        uint_t horizon_num;
        uint_vector_t horizons;
        uint_vector_t scenarios; /* holds indexes of scenarios for each time horizon */

        /* exclusions */
        uint_vector_2d_t seasons; /* two dimensional vector; row=season, columns=horizons in season */
        pair_to_uint_t exclusion_seasons; /* map pair of interventions to index in season table */
        uint_pair_vector_t exclusions; /* vector of pairs of interventions */
        uint_vector_2d_t time_exclusions; /* map time horizon to vector of indexes into exclusions table */
        uint_vector_2d_t exclusions_by_interventions; /* map intervention to vector of interventions in exclusion */
        uint_set_t interventions_with_exclusions; /* set of interventions, that are in some exclusion */

        uint_vector_t t_max; /* latest horizon to start corresponding intervention */

        fitness_t_ quantile;
        fitness_t_ alpha;
        uint_t computation_time;

        vector<pair<uint_t, fitness_t_>> avg_deltas; /* vector of pairs <intervention, avg_delta>, sorted by avg_deltas */
        vector<pair<uint_t, fitness_t_>> avg_costs; /* vector of pairs <intervention, avg_delta>, sorted by avg_costs */
        vector<pair<uint_t, fitness_t_>> avg_rds; /* vector of pairs <intervention, avg_delta>, sorted by avg_rds */

    /* adds exlusion to list of exclusions and links it to all horizons that are present in its season */
        void add_exclusion_pair_to_season_horizons(uint_t id1, uint_t id2, uint_t season);
    public:
        Instance();
        ~Instance();

        /* prepare products to iprove otaining indices to arrays */
        void prepare_products(uint_t i, uint_t r, uint_t t);
        void allocate_arrays();

        fitness_t_ *u;
        fitness_t_ *l;

        uint_t *delta;

        half_fitness_t_ *r;

        half_fitness_vector_t *risk;

        uint_t add_resource(string name); /* adds resource with passed name to maps and vector */
        /* translate resource to id and vice versa */
        uint_t get_resource_id(string resource);
        string get_resource(uint_t id);
        const uint_vector_t& get_resources() const;

        uint_t add_intervention(string name); /* adds intervention with passed name to maps and vector */
        /* translate intervention to id and vice versa */
        uint_t get_intervention_id(string intervention);
        string get_intervention(uint_t id);
        const uint_vector_t& get_interventions() const;
        uint_t get_intervention_count();

        const uint_set_t &get_interventions_resources(uint_t intervention) const;

        void fill_horizons(uint_t last_value); /* creates vector (1, 2, ..., last_value) */
        const uint_vector_t& get_horizons() const; /* returns vector that contains all horizons sorted in ascending order <1,n> */
        const uint_t get_horizon_num(); /* returns range of planification */

        void add_scenario(uint_t scenario); /* adds passed scenario to end of the vector */
        uint_t get_scenarios(uint_t horizon); /* returns number of scenarios in given horizon */
        uint_vector_t get_scenarios_vector(uint_t horizon); /* returns vector that contains individual scnarios in given time in ascending order */
        uint_t get_scenarios_count(); /* returns number of scenarios */
        uint_t get_max_scenarios();

        void set_eval_parameters(fitness_t_ quantile, fitness_t_ alpha, uint_t comp_time); /* loads parameters used for evaluation */

        void append_season(); /* adds new empty line to season table */
        void append_to_season(uint_t season, uint_t value); /* adds value to season table in row specified by season param */
        void combine_seasons(uint_t target_season, uint_t source_season); /* adds all elements of source_season to target_season */
        void sort_season(uint_t season); /* sorts season in ascending order */

        /* handle inserting and retrieving exlusions */
        void add_exclusion(string intervention1, string intervention2, uint_t season_id);
        void add_exclusion(uint_t intervention1, uint_t intervention2, uint_t season_id);
        /* gets vector that contains times where two passed interventions cannot be conducted at once */
        uint_vector_t get_exclusion_season(uint_t intervetion1, uint_t intervention2);
        uint_vector_t get_exclusion_season(string intervention1, string intervention2);
        /* get vector of pairs of interventions that are excluded in passed time_horizon */
        const uint_pair_vector_t get_exclusions(uint_t time_horizon) const;
        const uint_pair_vector_t get_exclusion_pairs() const;
        /* get vector of interventions that are in exclusion with given intervention */
        const uint_vector_t& get_excluded(uint_t intervention) const;
        /* get a set of interventions, that are in some exclusion */
        const uint_set_t& get_interventions_with_exclusions() const;

        uint_t get_ul_index(string resource, uint_t horizon);
        uint_t get_ul_index(uint_t resource, uint_t horizon);

        uint_t get_delta_index(string intervention, uint_t horizon);
        uint_t get_delta_index(uint_t intervention, uint_t horizon);

        void add_r(uint_t intervention, uint_t resource, uint_t index, half_fitness_t_ r);

        uint_t get_r_index(string intervention, uint_t start_horizon, string resource, uint_t horizon);
        uint_t get_r_index(uint_t intervention, uint_t start_horizon, uint_t resource, uint_t horizon);

        uint_t get_risk_index(uint_t intervention, uint_t start_horizon, uint_t horizon);
        uint_t get_risk_index(string intervention, uint_t start_horizon, uint_t horizon);

        /* add latest time horizon to start passed intervention */
        void add_t_max(string intervention, uint_t t_max);
        void add_t_max(uint_t intervention, uint_t t_max);
        /* get latest time horizon to start passed intervention */
        uint_t get_t_max(string intervention);
        uint_t get_t_max(uint_t intervention);

        /* get evaluation parameters */
        fitness_t_ get_quantile();
        fitness_t_ get_alpha();
        uint_t get_computation_time();

        void set_avg_deltas(vector<pair<uint_t, fitness_t_>> avg_deltas);
        void set_avg_costs(vector<pair<uint_t, fitness_t_>> avg_costs);
        void set_avg_rds(vector<pair<uint_t, fitness_t_>> avg_rds);
        const vector<pair<uint_t, fitness_t_>>& get_avg_deltas() const;
        const vector<pair<uint_t, fitness_t_>>& get_avg_costs() const;
        const vector<pair<uint_t, fitness_t_>>& get_avg_rds() const;
    ;
};
}
/* adds uint value to vector */
void add_to_vector(uint_vector_t *vec, uint_t value);

/* adds uint value to set */
void add_to_set(uint_set_t *set, uint_t value);

/* adds name and id to maps in both directions */
void add_to_translate_maps(string_to_uint_t *id_map, uint_to_string_t *name_map, string name, uint_t id);

#endif
