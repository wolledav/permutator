#include "instance.hpp"


/* adds exlusion to list of exclusions and links it to all horizons that are present in its season */
void ins::Instance::add_exclusion_pair_to_season_horizons(uint_t id1, uint_t id2, uint_t season) {
    this->exclusions.push_back({id1, id2});
    uint_t excl_index = this->exclusions.size() - 1;
    for (int horizon : this->seasons[season]) {
        this->time_exclusions[TO_INDEX(horizon)].push_back(excl_index);
    }
}

/* constructor */
ins::Instance::Instance() {
    /* set size of of exlusion 2d vector to maximal horizon size (specific value not known in compile time) */
    this->time_exclusions.resize(MAX_HORIZONS);
    this->exclusions_by_interventions.resize(MAX_INTERVENTIONS);
}

/* destructor */
ins::Instance::~Instance() {
    delete[] this->u;
    delete[] this->l;
    delete[] this->delta;
    delete[] this->r;
    delete[] this->risk;
}

void ins::Instance::prepare_products(uint_t i, uint_t r, uint_t t) {
    this->rxt = r * t;
    this->ixt = i * t;
    this->ixt_ = i * t;
    this->ixt_xt = i * t * t;
    this->ixt_xr = i * t * r;
    this->ixt_xrxt = i * t * r * t;
    this->t_xrxt = t * r * t;
    this->t_xt = t * t;
}

void ins::Instance::allocate_arrays() {
    this->u = new fitness_t_[this->rxt];
    this->l = new fitness_t_[this->rxt];
    this->delta = new uint_t[this->ixt];
    this->r = new half_fitness_t_[this->ixt_xrxt]();
    this->risk = new half_fitness_vector_t[this->ixt_xt];
}

/*
 * adds resource name to map in both directions
 * adds its id to resource set
 * returns id of the resource
 */
uint_t ins::Instance::add_resource(string resource) {
    add_to_translate_maps(&(this->resource_to_id), &(this->id_to_resource), resource, this->resource_count);
    add_to_vector(&(this->resources), this->resource_count);
    return this->resource_count++;
}

uint_t ins::Instance::get_resource_id(string resource) {
    return this->resource_to_id[resource];
}

string ins::Instance::get_resource(uint_t id) {
    return this->id_to_resource[id];
}

const uint_vector_t& ins::Instance::get_resources() const {
    return this->resources;
}

/*
 * adds intervention name to map in both directions
 * adds its id to intervention set\
 * returns id of the intervention
 */
uint_t ins::Instance::add_intervention(string intervention) {
    add_to_translate_maps(&(this->intervention_to_id), &(this->id_to_intervention), intervention, this->intervention_count);
    add_to_vector(&(this->interventions), this->intervention_count);
    this->resources_used_by_intervention.push_back(uint_set_t());
    return this->intervention_count++;
}

uint_t ins::Instance::get_intervention_id(string intervention) {
    return this->intervention_to_id[intervention];
}

string ins::Instance::get_intervention(uint_t id) {
    return this->id_to_intervention[id];
}

const uint_vector_t& ins::Instance::get_interventions() const {
    return this->interventions;
}

uint_t ins::Instance::get_intervention_count() {
    return this->intervention_count - 1;
}

const uint_set_t &ins::Instance::get_interventions_resources(uint_t intervention) const {
    return this->resources_used_by_intervention[TO_INDEX(intervention)];
}

/* creates vector in format (1, 2, ..., last_value) */
void ins::Instance::fill_horizons(uint_t last_value) {
    this->horizon_num = last_value;
    for (int i = 1; i <= last_value; ++i) {
        add_to_vector(&(this->horizons), i);
    }
}

/* returns vector that contains all horizons sorted in ascending order <1,n> */
const uint_vector_t& ins::Instance::get_horizons() const {
    return this->horizons;
}

const uint_t ins::Instance::get_horizon_num() {
    return this->horizon_num;
}

/* adds scenario to end of scenarios vector */
void ins::Instance::add_scenario(uint_t scenario) {
    this->scenarios.push_back(scenario);
}

/* gets number of scenarios in given time horizon */
uint_t ins::Instance::get_scenarios(uint_t horizon) {
    return this->scenarios[TO_INDEX(horizon)];
}

/* gets vector containing all scenarios in given time in ascending order */
uint_vector_t ins::Instance::get_scenarios_vector(uint_t horizon) {
    uint_vector_t vector;
    for (uint_t scenario = 1; scenario <= this->scenarios[TO_INDEX(horizon)]; ++scenario) {
        vector.push_back(scenario);
    }
    return vector;
}

uint_t ins::Instance::get_scenarios_count() {
    return this->scenarios.size();
}

uint_t ins::Instance::get_max_scenarios() {
    uint_t max = 0;
    for (uint_t scenario : this->scenarios) {
        if (scenario > max) max = scenario;
    }
    return max;
}

/* sets evaluation paramters */
void ins::Instance::set_eval_parameters(fitness_t_ quantile, fitness_t_ alpha, uint_t comp_time) {
    this->quantile = quantile;
    this->alpha = alpha;
    this->computation_time = comp_time;
}

/* creates new row in season table */
void ins::Instance::append_season() {
    uint_vector_t vec;
    this->seasons.push_back(vec);
}

/* adds new item to the end of a row in season table */
void ins::Instance::append_to_season(uint_t season, uint_t value) {
    if (season < this->seasons.size()) {
        this->seasons[season].push_back(value);
    } else util::throw_err("Season does not exist");
}

/* adds all elements of source_season to target_season */
void ins::Instance::combine_seasons(uint_t target_season, uint_t source_season) {
    for (uint_t time : this->seasons[source_season]) {
        this->seasons[target_season].push_back(time);
    }
}

/* sorts season in ascending order */
void ins::Instance::sort_season(uint_t season) {
    stable_sort(this->seasons[season].begin(), this->seasons[season].end());
}

/* maps row index in season table to pair of interventions */
void ins::Instance::add_exclusion(string intervention1, string intervention2, uint_t season_id) {
    // sort pair to be in format {smaller, larger}
    this->add_exclusion(this->get_intervention_id(intervention1), this->get_intervention_id(intervention2), season_id);
}

/* maps row index in season table to pair of interventions */
void ins::Instance::add_exclusion(uint_t intervention1, uint_t intervention2, uint_t season_id) {
    struct pair_key key = {min(intervention1, intervention2), max(intervention1, intervention2)};
    //struct pair_key key = (intervention1 < intervention2) ? (struct pair_key){intervention1, intervention2} : (struct pair_key){intervention2, intervention1};
    this->add_exclusion_pair_to_season_horizons(key.a, key.b, season_id);
    this->exclusion_seasons[key] = season_id;
    this->exclusions_by_interventions[intervention1].push_back(intervention2);
    this->exclusions_by_interventions[intervention2].push_back(intervention1);
    this->interventions_with_exclusions.insert(intervention1);
    this->interventions_with_exclusions.insert(intervention2);
}

/* gets vector that contains times where two passed interventions cannot be conducted at once */
uint_vector_t ins::Instance::get_exclusion_season(string intervention1, string intervention2) {
    uint_t id1 = this->get_intervention_id(intervention1);
    uint_t id2 = this->get_intervention_id(intervention2);
    // sort pair to be in format {smaller, larger}
    struct pair_key key = {min(id1, id2), max(id1, id2)};
    //struct pair_key key = (id1 < id2) ? (struct pair_key){id1, id2} : (struct pair_key){id2, id1};
    return this->seasons[this->exclusion_seasons[key]];
}

/* gets vector that contains times where two passed interventions cannot be conducted at once */
uint_vector_t ins::Instance::get_exclusion_season(uint_t intervention1, uint_t intervention2) {
    // sort pair to be in format {smaller, larger}
    struct pair_key key = {min(intervention1, intervention2), max(intervention1, intervention2)};
    //struct pair_key key = (intervention1 < intervention2) ? (struct pair_key){intervention1, intervention2} : (struct pair_key){intervention2, intervention1};
    return this->seasons[this->exclusion_seasons[key]];
}

/* get vector of pairs of interventions that are excluded in passed time_horizon */
const uint_pair_vector_t ins::Instance::get_exclusions(uint_t time_horizon) const {
    uint_pair_vector_t ret;
    for (uint_t index : this->time_exclusions[TO_INDEX(time_horizon)]) {
        ret.push_back(this->exclusions[index]);
    }
    return ret;
}

const uint_pair_vector_t ins::Instance::get_exclusion_pairs() const {
    return this->exclusions;
}

const uint_vector_t &ins::Instance::get_excluded(uint_t intervention) const {
    return this->exclusions_by_interventions[intervention];
}

const uint_set_t &ins::Instance::get_interventions_with_exclusions() const {
    return this->interventions_with_exclusions;
}

uint_t ins::Instance::get_ul_index(string resource, uint_t horizon) {
    return TO_INDEX(this->get_resource_id(resource)) * this->horizon_num + TO_INDEX(horizon);
}

uint_t ins::Instance::get_ul_index(uint_t resource, uint_t horizon) {
    return TO_INDEX(resource) * this->horizon_num + TO_INDEX(horizon);
}

uint_t ins::Instance::get_delta_index(string intervention, uint_t horizon) {
    return TO_INDEX(this->get_intervention_id(intervention)) * this->horizon_num + TO_INDEX(horizon);
}

uint_t ins::Instance::get_delta_index(uint_t intervention, uint_t horizon) {
    return TO_INDEX(intervention) * this->horizon_num + TO_INDEX(horizon);
}

void ins::Instance::add_r(uint_t intervention, uint_t resource, uint_t index, half_fitness_t_ r) {
    this->resources_used_by_intervention[TO_INDEX(intervention)].insert(resource);
    this->r[index] = r;
}

uint_t ins::Instance::get_r_index(string intervention, uint_t start_horizon, string resource, uint_t horizon) {
    return TO_INDEX(this->get_intervention_id(intervention)) * this->t_xrxt + TO_INDEX(start_horizon) * this->rxt + TO_INDEX(this->get_resource_id(resource)) * this->horizon_num + TO_INDEX(horizon);
}

uint_t ins::Instance::get_r_index(uint_t intervention, uint_t start_horizon, uint_t resource, uint_t horizon) {
    return TO_INDEX(intervention) * this->t_xrxt + TO_INDEX(start_horizon) * this->rxt + TO_INDEX(resource) * this->horizon_num + TO_INDEX(horizon);
}

uint_t ins::Instance::get_risk_index(string intervention, uint_t start_horizon, uint_t horizon) {
    return TO_INDEX(this->get_intervention_id(intervention)) * this->t_xt + TO_INDEX(start_horizon) * this->horizon_num + TO_INDEX(horizon);
}

uint_t ins::Instance::get_risk_index(uint_t intervention, uint_t start_horizon, uint_t horizon) {
    return TO_INDEX(intervention) * this->t_xt + TO_INDEX(start_horizon) * this->horizon_num + TO_INDEX(horizon);
}

void ins::Instance::add_t_max(string intervention, uint_t t_max) {
    this->add_t_max(this->get_intervention_id(intervention), t_max);
}

void ins::Instance::add_t_max(uint_t intervention, uint_t t_max) {
    while (intervention >= this->t_max.size()) {
        this->t_max.push_back(0);
    }
    this->t_max[intervention] = t_max;
}

uint_t ins::Instance::get_t_max(string intervention) {
    return this->get_t_max(this->get_intervention_id(intervention));
}

uint_t ins::Instance::get_t_max(uint_t intervention) {
    return this->t_max[intervention];
}

fitness_t_ ins::Instance::get_quantile() {
    return this->quantile;
}

fitness_t_ ins::Instance::get_alpha() {
    return this->alpha;
}

uint_t ins::Instance::get_computation_time() {
    return this->computation_time;
}

void ins::Instance::set_avg_deltas(vector<pair<uint_t, fitness_t_>> avg_deltas) {
    this->avg_deltas = std::move(avg_deltas);
}

void ins::Instance::set_avg_costs(vector<pair<uint_t, fitness_t_>> avg_costs) {
    this->avg_costs = std::move(avg_costs);
}

void ins::Instance::set_avg_rds(vector<pair<uint_t, fitness_t_>> avg_rds) {
    this->avg_rds = std::move(avg_rds);
}

const vector<pair<uint_t, fitness_t_>> &ins::Instance::get_avg_costs() const {
    return this->avg_costs;
}

const vector<pair<uint_t, fitness_t_>> &ins::Instance::get_avg_rds() const {
    return this->avg_rds;
}

const vector<pair<uint_t, fitness_t_>> &ins::Instance::get_avg_deltas() const {
    return this->avg_deltas;
}

/* adds string and its id to maps (both directions) */
void add_to_translate_maps(string_to_uint_t *id_map, uint_to_string_t *name_map, string name, uint_t id) {
    (*id_map).insert(pair<string, uint_t>(name, id));
    (*name_map).insert(pair<uint_t, string>(id, name));
}

void add_to_set(uint_set_t *s, uint_t value) {
    (*s).insert(value);
}

/* adds value to the end of the list */
void add_to_vector(uint_vector_t *vec, uint_t value) {
    (*vec).push_back(value);
}
