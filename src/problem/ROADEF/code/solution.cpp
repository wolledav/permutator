#include "solution.hpp"

ROADEF::Solution::Solution(ROADEF::Instance *instance, std::default_random_engine *engine) {
    this->instance = instance;
    this->engine = engine;
    for (uint_t intervention : instance->get_interventions()) {
        this->unscheduled.insert(intervention);
    }
    /* resize fields according to indexed value (indexing from 1 -> size + 1) */
    uint_t horizons = this->instance->get_horizon_num();
    /* initialize state variables to 0 */
    this->mean_risk = 0;
    this->expected_excess = 0;
    this->final_objective = numeric_limits<fitness_t_>::max();
    this->workload_underuse = 0;
    this->workload_overuse = 0;
    this->total_resource_use = 0;
    // this->workload_usage = 0;
    this->exclusion_penalty = 0;
    // this->resource_usage.resize(resource_range);
    /* init vectors one item bigger to index from 1 and avoid adding */
    for (uint_t i = 0; i <= horizons; ++i) {
        this->mean_cumulative_risks.push_back(0.0);
        this->expected_excesses.push_back(0.0);
        this->exclusions_at_time.push_back(0);
    }
    uint_t resources = this->instance->get_resources().size();
    vector<fitness_t_> line_;
    this->resource_use.push_back(line_);
    this->resource_overuse.push_back(0.0);
    this->resource_underuse.push_back(0.0);
    uint_t l_idx = 0;
    for (uint_t r = 1; r <= resources; ++r) {
        vector<fitness_t_> line(horizons + 1, 0.0);
        this->resource_use.push_back(line);
        this->resource_overuse.push_back(0.0);
        this->resource_underuse.push_back(0.0);
        for (uint_t t = 1; t <= horizons; ++t) {
            fitness_t_ l = this->instance->l[l_idx++];
            this->resource_underuse[r] += l;
            this->workload_underuse += l;
        }
    }
    /* initialize risk_st, indexing from [1][1] */
    vector<fitness_t_> _line;
    this->risk_st.push_back(_line);
    for (uint_t t = 1; t <= horizons; ++t) {
        vector<fitness_t_> line(this->instance->get_scenarios(t) + 1, 0.0);
        this->risk_st.push_back(line);
    }
    this->extended_objective = BETA_LOWER * this->workload_underuse;
    this->restarts_cnt = 0;
    this->unscheduled_cnt = vector<uint_t> (instance->get_intervention_count() + 1, 0);
}

void ROADEF::Solution::schedule(uint_t intervention_id, uint_t start_time) {
    if (this->is_scheduled(intervention_id)) {
        cerr << "!! Trying to schedule intervention that is already scheduled " << this->instance->get_intervention(intervention_id) << " !!" << endl;
        exit(SCHEDULE_ERR);
    }
    this->start_times[intervention_id] = start_time;
    this->scheduled.insert(intervention_id);
    this->unscheduled.erase(intervention_id);
    this->update_state_on_schedule(intervention_id, start_time);
    this->update_extended_state_on_schedule(intervention_id, start_time);
}

void ROADEF::Solution::unschedule(uint_t intervention_id) {
    if (!this->is_scheduled(intervention_id)) {
        cerr << "!! Trying to unschedule intervention that is not scheduled, id: " << intervention_id << " !!" << endl;
        exit(SCHEDULE_ERR);
    }
    this->update_state_on_unschedule(intervention_id, this->start_times[intervention_id]);
    this->update_extended_state_on_unschedule(intervention_id, this->start_times[intervention_id]);
    this->start_times.erase(intervention_id);
    this->unscheduled.insert(intervention_id);
    this->scheduled.erase(intervention_id);
//    this->unscheduled_cnt[intervention_id] ++;
}

bool ROADEF::Solution::is_scheduled(uint_t intervention_id) {
    return this->scheduled.find(intervention_id) != this->scheduled.end();
}

uint_t ROADEF::Solution::get_start_time(uint_t intervention_id) {
    if (this->is_scheduled(intervention_id)) return this->start_times[intervention_id];
    else {
        cerr << "!! Trying to get start time of intervention that is not scheduled " << this->instance->get_intervention(intervention_id) << " !!" << endl;
        exit(SCHEDULE_ERR);
    }
}

uint_t ROADEF::Solution::get_unscheduled_count() {
    return (uint_t)this->unscheduled.size();
}

bool ROADEF::Solution::has_unscheduled() {
    return !(this->unscheduled.empty());
}

uint_t ROADEF::Solution::get_first_unscheduled() {
    return *(this->unscheduled.begin());
}

void ROADEF::Solution::save(string output_file_path) {
    std::ofstream output;
    output.open(output_file_path);
    if (!output.is_open()) util::throw_err("Error while opening output file");

#if SAVE_SOL_PROGRESS
    output << "extended_objective: " << extended_objective << endl;
#endif

    for (uint_to_uint_t::iterator iter = this->start_times.begin(); iter != this->start_times.end(); ++iter) {
        if (!this->instance->get_intervention(iter->first).empty()) {
            output << this->instance->get_intervention(iter->first) << " " << iter->second << endl;
        }
    }
    output.close();
}

bool ROADEF::Solution::is_valid() {
    return this->workload_overuse < NUMERIC_TOLERANCE &&
           this->workload_underuse < NUMERIC_TOLERANCE &&
           this->exclusion_penalty == 0;
}

Objective ROADEF::Solution::estimate_schedule(uint_t intervention_id, uint_t start_time) {
    if (this->is_scheduled(intervention_id)) {
        cerr << "!! Trying to estimate schedule of intervention that is already scheduled " << this->instance->get_intervention(intervention_id) << " !!" << endl;
        exit(SCHEDULE_ERR);
    }
    Objective o;
    this->estimate_state_on_schedule(intervention_id, start_time, &o);
    this->estimate_extended_state_on_schedule(intervention_id, start_time, &o);
    return o;
}

Objective ROADEF::Solution::estimate_unschedule(uint_t intervention_id) {
    if (!this->is_scheduled(intervention_id)) {
        cerr << "!! Trying to estimate unschedule of intervention that is not scheduled " << this->instance->get_intervention(intervention_id) << " !!" << endl;
        exit(SCHEDULE_ERR);
    }
    Objective o;
    if (!this->is_scheduled(intervention_id)) return o;
    this->estimate_state_on_unschedule(intervention_id, this->start_times[intervention_id], &o);
    this->estimate_extended_state_on_unschedule(intervention_id, this->start_times[intervention_id], &o);
    return o;
}

void ROADEF::Solution::update_state_on_schedule(uint_t scheduled_intervention, uint_t start_time) {
    uint_t scenarios;
    uint_t horizons = this->instance->get_horizon_num();
    fitness_t_ risk_st;
    fitness_t_ risk_t_;
    fitness_t_ Q_tt;
    fitness_t_ excess;
    uint_t Qtt_index;
    array<fitness_t_, ARR_SIZE> arr;
    vector<fitness_t_> vec;
    uint_t risk_in_s_idx = this->instance->get_risk_index(scheduled_intervention, start_time, start_time);
    half_fitness_vector_t risk_in_s;
    for (uint_t t = start_time; t < start_time + this->instance->delta[this->instance->get_delta_index(scheduled_intervention, start_time)]; ++t) {
        // cout << "t: " << t << endl;
        scenarios = this->instance->get_scenarios(t);
        risk_in_s = this->instance->risk[risk_in_s_idx++];
        for (uint_t s = 1; s <= scenarios; ++s) {
            // cout << "s: " << s << endl;
            risk_st = risk_in_s[TO_INDEX(s)];
            risk_t_ = (risk_st / scenarios);
            this->risk_st[t][s] += risk_st;
            this->mean_cumulative_risks[t] += risk_t_;
            this->mean_risk += (risk_t_ / horizons);
        }
        Qtt_index = ceil(scenarios * this->instance->get_quantile());
        if (scenarios < ARR_SIZE) {
            copy(this->risk_st[t].begin(), this->risk_st[t].end(), arr.begin());
            nth_element(arr.begin(), arr.begin() + Qtt_index, ARR_END_IT(arr.begin(), scenarios));
            Q_tt = arr[Qtt_index] - this->mean_cumulative_risks[t];
        } else {
            vec = this->risk_st[t];
            nth_element(vec.begin(), vec.begin() + Qtt_index, vec.end());
            Q_tt = vec[Qtt_index] - this->mean_cumulative_risks[t];
        }
        excess = Q_tt < 0 ? 0 : Q_tt;
        this->expected_excess += ((excess - this->expected_excesses[t]) / horizons);
        this->expected_excesses[t] = excess;
    }
    this->final_objective = this->instance->get_alpha() * this->mean_risk + (1 - this->instance->get_alpha()) * this->expected_excess;
}

void ROADEF::Solution::update_extended_state_on_schedule(uint scheduled_intervention, uint_t start_time) {
    fitness_t_ before;
    fitness_t_ after;
    fitness_t_ gain;
    fitness_t_ max;
    fitness_t_ min;
    uint_t end = start_time + this->instance->delta[this->instance->get_delta_index(scheduled_intervention, start_time)];
    uint_t r_idx;
    uint_t ul_idx;
    for (uint_t r : this->instance->get_interventions_resources(scheduled_intervention)) {
        r_idx = this->instance->get_r_index(scheduled_intervention, start_time, r, start_time);
        ul_idx = this->instance->get_ul_index(r, start_time);
        for (uint_t t = start_time; t < end; ++t) {
            before = this->resource_use[r][t];
            gain = this->instance->r[r_idx++];
            after = before + gain;
            max = this->instance->u[ul_idx];
            min = this->instance->l[ul_idx++];
            if (before < min) {
                if (after < min - NUMERIC_TOLERANCE) {
                    this->resource_underuse[r] -= gain;
                    this->resource_underuse[r] = std::max(this->resource_underuse[r], 0.0);
                    this->workload_underuse -= gain;
                    this->workload_underuse = std::max(this->workload_underuse, 0.0);
                } else {
                    this->resource_underuse[r] -= (min - before);
                    this->resource_underuse[r] = std::max(this->resource_underuse[r], 0.0);
                    this->workload_underuse -= (min - before);
                    this->workload_underuse = std::max(this->workload_underuse, 0.0);
                }
            }
            if (after > max + NUMERIC_TOLERANCE) {
                if (before > max) {
                    this->resource_overuse[r] += gain;
                    this->workload_overuse += gain;
                } else {
                    this->resource_overuse[r] += (after - max);
                    this->workload_overuse += (after - max);
                }
            }
            this->resource_use[r][t] = after;
            this->total_resource_use += gain;
        }
    }
    uint_t start2;
    uint_t end2;
    for (uint_t e : this->instance->get_excluded(scheduled_intervention)) {
        if (this->is_scheduled(e)) {
            start2 = this->start_times[e];
            end2 = start2 + this->instance->delta[this->instance->get_delta_index(e, start2)];
            for (uint_t t : this->instance->get_exclusion_season(scheduled_intervention, e)) {
                if (
                    t >= start_time && t >= start2 &&
                    t < end && t < end2
                ) {
                    this->exclusions_at_time[t]++;
                    this->exclusion_penalty++;
                }
            }
        }
    }
    this->extended_objective = this->final_objective + BETA_LOWER * this->workload_underuse + BETA_UPPER  * this->workload_overuse + GAMMA * this->exclusion_penalty;
}

void ROADEF::Solution::update_state_on_unschedule(uint_t unscheduled_intervention, uint_t start_time) {
    uint_t scenarios;
    uint_t horizons = this->instance->get_horizon_num();
    fitness_t_ risk_st;
    fitness_t_ risk_t_;
    fitness_t_ Q_tt;
    fitness_t_ excess;
    uint_t Qtt_index;
    array<fitness_t_, ARR_SIZE> arr;
    vector<fitness_t_> vec;
    uint_t risk_in_s_idx = this->instance->get_risk_index(unscheduled_intervention, start_time, start_time);
    half_fitness_vector_t risk_in_s;
    for (uint_t t = start_time; t < start_time + this->instance->delta[this->instance->get_delta_index(unscheduled_intervention, start_time)]; ++t) {
        scenarios = this->instance->get_scenarios(t);
        risk_in_s = this->instance->risk[risk_in_s_idx++];
        for (uint_t s = 1; s <= scenarios; ++s) {
            risk_st = risk_in_s[TO_INDEX(s)];
            risk_t_ = (risk_st / scenarios);
            this->risk_st[t][s] -= risk_st;
            this->mean_cumulative_risks[t] -= risk_t_;
            this->mean_risk -= (risk_t_ / horizons);
        }
        Qtt_index = ceil(scenarios * this->instance->get_quantile());
        if (scenarios < ARR_SIZE) {
            copy(this->risk_st[t].begin(), this->risk_st[t].end(), arr.begin());
            nth_element(arr.begin(), arr.begin() + Qtt_index, ARR_END_IT(arr.begin(), scenarios));
            Q_tt = arr[Qtt_index] - this->mean_cumulative_risks[t];
        } else {
            vec = this->risk_st[t];
            nth_element(vec.begin(), vec.begin() + Qtt_index, vec.end());
            Q_tt = vec[Qtt_index] - this->mean_cumulative_risks[t];
        }
        excess = Q_tt < 0 ? 0 : Q_tt;
        this->expected_excess += ((excess - this->expected_excesses[t]) / horizons);
        this->expected_excesses[t] = excess;
    }
    this->final_objective = this->instance->get_alpha() * this->mean_risk + (1 - this->instance->get_alpha()) * this->expected_excess;
}

void ROADEF::Solution::update_extended_state_on_unschedule(uint unscheduled_intervention, uint_t start_time) {
    fitness_t_ before;
    fitness_t_ after;
    fitness_t_ loss;
    fitness_t_ max;
    fitness_t_ min;
    uint_t r_idx;
    uint_t ul_idx;
    uint_t end = start_time + this->instance->delta[this->instance->get_delta_index(unscheduled_intervention, start_time)];
    for (uint_t r : this->instance->get_interventions_resources(unscheduled_intervention)) {
        r_idx = this->instance->get_r_index(unscheduled_intervention, start_time, r, start_time);
        ul_idx = this->instance->get_ul_index(r, start_time);
        for (uint_t t = start_time; t < end; ++t) {
            before = this->resource_use[r][t];
            loss = this->instance->r[r_idx++];
            after = before - loss;
            max = this->instance->u[ul_idx];
            min = this->instance->l[ul_idx++];
            if (after < min - NUMERIC_TOLERANCE) {
                if (before < min) {
                    this->resource_underuse[r] += loss;
                    this->workload_underuse += loss;
                } else {
                    this->resource_underuse[r] += (min - after);
                    this->workload_underuse += (min - after);
                }
            }
            if (before > max) {
                if (after > max + NUMERIC_TOLERANCE) {
                    this->resource_overuse[r] -= loss;
                    this->resource_overuse[r] = std::max(this->resource_overuse[r], 0.0);
                    this->workload_overuse -= loss;
                    this->workload_overuse = std::max(this->workload_overuse, 0.0);
                } else {
                    this->resource_overuse[r] -= (before - max);
                    this->resource_overuse[r] = std::max(this->resource_overuse[r], 0.0);
                    this->workload_overuse -= (before - max);
                    this->workload_overuse = std::max(this->workload_overuse, 0.0);
                }
            }
            this->resource_use[r][t] = after;
            this->total_resource_use -= loss;
        }
    }
    uint_t start2;
    uint_t end2;
    for (uint_t e : this->instance->get_excluded(unscheduled_intervention)) {
        if (this->is_scheduled(e)) {
            start2 = this->start_times[e];
            end2 = start2 + this->instance->delta[this->instance->get_delta_index(e, start2)];
            for (uint_t t : this->instance->get_exclusion_season(unscheduled_intervention, e)) {
                if (
                    t >= start_time && t >= start2 &&
                    t < end && t < end2
                ) {
                    this->exclusions_at_time[t]--;
                    this->exclusion_penalty--;
                }
            }
        }
    }
    this->extended_objective = this->final_objective + BETA_LOWER * this->workload_underuse + BETA_UPPER  * this->workload_overuse + GAMMA * this->exclusion_penalty;
}

void ROADEF::Solution::estimate_state_on_schedule(uint_t scheduled_intervention, uint_t start_time, Objective *objective) {
    fitness_t_ mean_risk = this->mean_risk;
    fitness_t_ expected_excess = this->expected_excess;
    fitness_t_ risk_st;
    fitness_t_ risk_t_;
    uint_t scenarios;
    uint_t horizons = this->instance->get_horizon_num();
    uint_t Qtt_index;
    fitness_t_ Qtt;
    array<fitness_t_, ARR_SIZE> arr;
    vector<fitness_t_> vec;
    fitness_t_ mean_cumulative_risk;
    uint_t risk_in_s_idx = this->instance->get_risk_index(scheduled_intervention, start_time, start_time);
    half_fitness_vector_t risk_in_s;
    for (uint_t t = start_time; t < start_time + this->instance->delta[this->instance->get_delta_index(scheduled_intervention, start_time)]; ++t) {
        risk_in_s = this->instance->risk[risk_in_s_idx++];
        scenarios = this->instance->get_scenarios(t);
        mean_cumulative_risk = this->mean_cumulative_risks[t];
        Qtt_index = ceil(scenarios * this->instance->get_quantile());
        if (scenarios < ARR_SIZE) {
            copy(this->risk_st[t].begin(), this->risk_st[t].end(), arr.begin());
            for (uint_t s = 1; s <= scenarios; ++s) {
                risk_st = risk_in_s[TO_INDEX(s)];
                risk_t_ = risk_st / scenarios;
                arr[s] += risk_st;
                mean_cumulative_risk += risk_t_;
                mean_risk += risk_t_ / horizons;
            }
            nth_element(arr.begin(), arr.begin() + Qtt_index, ARR_END_IT(arr.begin(), scenarios));
            Qtt = arr[Qtt_index] - mean_cumulative_risk;
        } else {
            vec = this->risk_st[t];
            for (uint_t s = 1; s <= scenarios; ++s) {
                risk_st = risk_in_s[TO_INDEX(s)];
                risk_t_ = risk_st / scenarios;
                vec[s] += risk_st;
                mean_cumulative_risk += risk_t_;
                mean_risk += risk_t_ / horizons;
            }
            nth_element(vec.begin(), vec.begin() + Qtt_index, vec.end());
            Qtt = vec[Qtt_index] - mean_cumulative_risk;
        }
        expected_excess += (((Qtt > 0 ? Qtt : 0) - this->expected_excesses[t]) / horizons);
    }
    objective->mean_risk = mean_risk;
    objective->expected_excess = expected_excess;
    objective->final_objective = this->instance->get_alpha() * mean_risk + (1 - this->instance->get_alpha()) * expected_excess;
}

void ROADEF::Solution::estimate_extended_state_on_schedule(uint scheduled_intervention, uint_t start_time, Objective *objective) {
    fitness_t_ before;
    fitness_t_ after;
    fitness_t_ gain;
    fitness_t_ max;
    fitness_t_ min;
    fitness_t_ workload_overuse = this->workload_overuse;
    fitness_t_ workload_underuse = this->workload_underuse;
    fitness_t_ total_resource_use = this->total_resource_use;
    uint_t exclusion_penalty = this->exclusion_penalty;
    uint_t r_idx;
    uint_t ul_idx;
    uint_t end = start_time + this->instance->delta[this->instance->get_delta_index(scheduled_intervention, start_time)];
    for (uint_t r : this->instance->get_interventions_resources(scheduled_intervention)) {
        r_idx = this->instance->get_r_index(scheduled_intervention, start_time, r, start_time);
        ul_idx = this->instance->get_ul_index(r, start_time);
        for (uint_t t = start_time; t < end; ++t) {
            before = this->resource_use[r][t];
            gain = this->instance->r[r_idx++];
            after = before + gain;
            max = this->instance->u[ul_idx];
            min = this->instance->l[ul_idx++];
            if (before < min) {
                if (after < min - NUMERIC_TOLERANCE) {
                    workload_underuse -= gain;
                    workload_underuse = std::max(workload_underuse, 0.0);
                } else {
                    workload_underuse -= (min - before);
                    workload_underuse = std::max(workload_underuse, 0.0);
                }
            }
            if (after > max + NUMERIC_TOLERANCE) {
                if (before > max) {
                    workload_overuse += gain;
                } else {
                    workload_overuse += (after - max);
                }
            }
            total_resource_use += gain;
        }
    }
    uint_t start2;
    uint_t end2;
    for (uint_t e : this->instance->get_excluded(scheduled_intervention)) {
        if (this->is_scheduled(e)) {
            start2 = this->start_times[e];
            end2 = start2 + this->instance->delta[this->instance->get_delta_index(e, start2)];
            for (uint_t t : this->instance->get_exclusion_season(scheduled_intervention, e)) {
                if (
                    t >= start_time && t >= start2 &&
                    t < end && t < end2
                ) {
                    exclusion_penalty++;
                }
            }
        }
    }
    objective->total_resource_use = total_resource_use;
    objective->workload_overuse = workload_overuse;
    objective->workload_underuse = workload_underuse;
    objective->exclusion_penalty = exclusion_penalty;
    objective->extended_objective = objective->final_objective + BETA_LOWER * workload_underuse + BETA_UPPER  * workload_overuse + GAMMA * exclusion_penalty;
}

void ROADEF::Solution::estimate_state_on_unschedule(uint_t unscheduled_intervention, uint_t start_time, Objective *objective) {
    fitness_t_ mean_risk = this->mean_risk;
    fitness_t_ expected_excess = this->expected_excess;
    fitness_t_ risk_st;
    fitness_t_ risk_t_;
    uint_t scenarios;
    uint_t horizons = this->instance->get_horizon_num();
    uint_t Qtt_index;
    fitness_t_ Qtt;
    array<fitness_t_, ARR_SIZE> arr;
    vector<fitness_t_> vec;
    fitness_t_ mean_cumulative_risk;
    uint_t risk_in_s_idx = this->instance->get_risk_index(unscheduled_intervention, start_time, start_time);
    half_fitness_vector_t risk_in_s;
    for (uint_t t = start_time; t < start_time + this->instance->delta[this->instance->get_delta_index(unscheduled_intervention, start_time)]; ++t) {
        risk_in_s = this->instance->risk[risk_in_s_idx++];
        scenarios = this->instance->get_scenarios(t);
        mean_cumulative_risk = this->mean_cumulative_risks[t];
        Qtt_index = ceil(scenarios * this->instance->get_quantile());
        if (scenarios < ARR_SIZE) {
            copy(this->risk_st[t].begin(), this->risk_st[t].end(), arr.begin());
            for (uint_t s = 1; s <= scenarios; ++s) {
                risk_st = risk_in_s[TO_INDEX(s)];
                risk_t_ = risk_st / scenarios;
                arr[s] -= risk_st;
                mean_cumulative_risk -= risk_t_;
                mean_risk -= risk_t_ / horizons;
            }
            nth_element(arr.begin(), arr.begin() + Qtt_index, ARR_END_IT(arr.begin(), scenarios));
            Qtt = arr[Qtt_index] - mean_cumulative_risk;
        } else {
            vec = this->risk_st[t];
            for (uint_t s = 1; s <= scenarios; ++s) {
                risk_st = risk_in_s[TO_INDEX(s)];
                risk_t_ = risk_st / scenarios;
                vec[s] -= risk_st;
                mean_cumulative_risk -= risk_t_;
                mean_risk -= risk_t_ / horizons;
            }
            nth_element(vec.begin(), vec.begin() + Qtt_index, vec.end());
            Qtt = vec[Qtt_index] - mean_cumulative_risk;
        }
        expected_excess += (((Qtt > 0 ? Qtt : 0) - this->expected_excesses[t]) / horizons);
    }
    objective->mean_risk = mean_risk;
    objective->expected_excess = expected_excess;
    objective->final_objective = this->instance->get_alpha() * mean_risk + (1 - this->instance->get_alpha()) * expected_excess;
}

void ROADEF::Solution::estimate_extended_state_on_unschedule(uint unscheduled_intervention, uint_t start_time, Objective *objective) {
    fitness_t_ before;
    fitness_t_ after;
    fitness_t_ loss;
    fitness_t_ max;
    fitness_t_ min;
    fitness_t_ workload_overuse = this->workload_overuse;
    fitness_t_ workload_underuse = this->workload_underuse;
    uint_t exclusion_penalty = this->exclusion_penalty;
    fitness_t_ total_resource_use = this->total_resource_use;
    uint_t r_idx;
    uint_t ul_idx;
    uint_t end = start_time + this->instance->delta[this->instance->get_delta_index(unscheduled_intervention, start_time)];
    for (uint_t r : this->instance->get_interventions_resources(unscheduled_intervention)) {
        r_idx = this->instance->get_r_index(unscheduled_intervention, start_time, r, start_time);
        ul_idx = this->instance->get_ul_index(r, start_time);
        for (uint_t t = start_time; t < end; ++t) {
            before = this->resource_use[r][t];
            loss = this->instance->r[r_idx++];
            after = before - loss;
            max = this->instance->u[ul_idx];
            min = this->instance->l[ul_idx++];
            if (after < min - NUMERIC_TOLERANCE) {
                if (before < min) {
                    workload_underuse += loss;
                } else {
                    workload_underuse += (min - after);
                }
            }
            if (before > max) {
                if (after > max + NUMERIC_TOLERANCE) {
                    workload_overuse -= loss;
                    workload_overuse = std::max(workload_overuse, 0.0);
                } else {
                    workload_overuse -= (before - max);
                    workload_overuse = std::max(workload_overuse, 0.0);
                }
            }
            total_resource_use -= loss;
        }
    }
    uint_t start2;
    uint_t end2;
    for (uint_t e : this->instance->get_excluded(unscheduled_intervention)) {
        if (this->is_scheduled(e)) {
            start2 = this->start_times[e];
            end2 = start2 + this->instance->delta[this->instance->get_delta_index(e, start2)];
            for (uint_t t : this->instance->get_exclusion_season(unscheduled_intervention, e)) {
                if (
                    t >= start_time && t >= start2 &&
                    t < end && t < end2
                ) {
                    exclusion_penalty--;
                }
            }
        }
    }
    objective->total_resource_use = total_resource_use;
    objective->workload_overuse = workload_overuse;
    objective->workload_underuse = workload_underuse;
    objective->exclusion_penalty = exclusion_penalty;
    objective->extended_objective = objective->final_objective + BETA_LOWER * workload_underuse + BETA_UPPER  * workload_overuse + GAMMA * exclusion_penalty;
}

void ROADEF::Solution::print() {
    for (uint_to_uint_t::iterator it = this->start_times.begin(); it != this->start_times.end(); ++it) {
        cout << this->instance->get_intervention(it->first) << " -> " << it->second << endl;
    }
}

void ROADEF::Solution::print_state() {
    printf("Restarts cnt: \t\t%u\n", this->restarts_cnt);
    printf("Valid: \t\t%u\n", this->is_valid());
    printf("Mean risk: \t\t%f\n", this->mean_risk);
    printf("Expected excess: \t%f\n", this->expected_excess);
    printf("Final objective:  \t%f\n", this->final_objective);
    printf("Total resource use: \t%f\n", this->total_resource_use);
    printf("Workload overuse: \t%f\n", this->workload_overuse);
    printf("Workload underuse: \t%f\n", this->workload_underuse);
    // printf("\tWorkload usage: \t%f\n", this->workload_usage);
    printf("Exclusion penalty: \t%u\n", this->exclusion_penalty);
    printf("Extended objective: \t%f\n\n", this->extended_objective);
}

void ROADEF::Solution::print_objective() {
    printf("%f\n", this->final_objective);
}

void ROADEF::Solution::print_extended_objective() {
    printf("%f\n", this->extended_objective);
}

bool ROADEF::Solution::check() {
    return this->undefined_intervention() &&
           this->unscheduled_intervention() &&
           this->start_time_out_of_range() &&
           this->start_time_out_of_intervention_range() &&
           this->resource_out_of_bounds() &&
           this->unmet_exclusion();
}

void ROADEF::Solution::info() {
    cerr << "Info:" << endl;
    cerr << '\t' << "Interventions number: " << this->instance->get_intervention_count() << endl;
    cerr << '\t' << "Scenarios number: " << this->instance->get_scenarios_count() << endl;
}

void ROADEF::Solution::eval() {
    this->compute_objectives();
    this->compute_final_objective();
    this->compute_workload_misuse();
    this->count_unmet_exclusions();
    this->compute_extended_objective();
    cout << "Evaluation:" << endl;
    this->print_state();
}

void ROADEF::Solution::print_unscheduled_cnt() {
    for (auto i:instance->get_interventions()) {
        cout << i << ": " << unscheduled_cnt[i] << endl;
    }
}

#if VERBOSE_SOLUTION

    void ROADEF::Solution::throw_message(const bool error, const string &label, const string &message) {}
    string ROADEF::Solution::time_bounds_details(const string &item, const uint_t start_time, const uint_t bound) { return ""; }
    string ROADEF::Solution::intervention_time_bounds_details(const string &item, const uint_t start_time, const uint_t bound) { return ""; }
    string ROADEF::Solution::workload_bounds_details(const bool upper, const string &item, const uint_t horizon, const fitness_t_ value, const uint_t bound) { return ""; }
    string ROADEF::Solution::exclusion_details(const string &first, const string &second, const uint_t horizon) { return ""; }

#else

    void ROADEF::Solution::throw_message(const bool correct, const string &label, const string &message) {
        cerr << (correct ? "#  >> Test passed <<" : "!  >> Error in solution <<") << endl;
        cerr << "\t" << label << " - " << message << endl;
    }

    string ROADEF::Solution::time_bounds_details(const string &item, const uint_t start_time, const uint_t bound) {
        ostringstream msg;
        msg << "Intervention '" << item << "' starts at time horizon " << start_time << " but planification range ends in " << bound << endl;
        return msg.str();
    }

    string ROADEF::Solution::intervention_time_bounds_details(const string &item, const uint_t start_time, const uint_t bound) {
        ostringstream msg;
        msg << "Intervention '" << item << "' starts at time horizon " << start_time << " but cannot start after " << bound << endl;
        return msg.str();
    }

    string ROADEF::Solution::workload_bounds_details(const bool upper, const string &item, const uint_t horizon, const fitness_t_ value, const uint_t bound) {
        ostringstream msg;
        msg << "Workload on resource '" << item << "' at time " << horizon << " is " << value << " and that is " << (upper ? "above the upper" : "below the lower") << " bound " << bound << endl;
        return msg.str();
    }

    string ROADEF::Solution::exclusion_details(const string &first, const string &second, const uint_t horizon) {
        ostringstream msg;
        msg << "Interventions '" << first << "' and '" << second << "' cannot be performed together at time horizon " << horizon << endl;
        return msg.str();
    }

#endif

bool ROADEF::Solution::undefined_intervention() {
    bool ret = CORRECT;
    for (uint_to_uint_t::iterator iter = this->start_times.begin(); iter != this->start_times.end(); ++iter) {
        if (find(this->instance->get_interventions().begin(), this->instance->get_interventions().end(), iter->first) == this->instance->get_interventions().end()) {
            ret = ERROR;
            this->throw_message(ret, "Undefined intervention", "intervention in solution is not present int instance dataset");
            break;
        }
    }
    if (ret) this->throw_message(ret, "Undefined intervention", "all interventions defined");
    return ret;
}

bool ROADEF::Solution::unscheduled_intervention() {
    bool ret = CORRECT;
    for (uint_t intervention : this->instance->get_interventions()) {
        if (this->start_times.find(intervention) == this->start_times.end()) {
            ret = ERROR;
            this->throw_message(ret, "Unscheduled intervention", this->instance->get_intervention(intervention));
            break;
        }
    }
    if (ret) this->throw_message(ret, "Unscheduled intervention", "all interventions scheduled");
    return ret;
}

bool ROADEF::Solution::start_time_out_of_range() {
    bool ret = CORRECT;
    for (uint_to_uint_t::iterator iter = this->start_times.begin(); iter != this->start_times.end(); ++iter) {
        if (iter->second < 1 || iter->second > this->instance->get_horizon_num()) {
            ret = ERROR;
            this->throw_message(ret, "Start time out of range", this->time_bounds_details(this->instance->get_intervention(iter->first), iter->second, this->instance->get_horizon_num()));
            break;
        }
    }
    if (ret) this->throw_message(ret, "Start time out of range", "all start times are in correct range");
    return ret;
}

bool ROADEF::Solution::start_time_out_of_intervention_range() {
    bool ret = CORRECT;
    for (uint_to_uint_t::iterator iter = this->start_times.begin(); iter != this->start_times.end(); ++iter) {
        if (iter->second > this->instance->get_t_max(iter->first)) {
            ret = ERROR;
            this->throw_message(ret, "", this->intervention_time_bounds_details(this->instance->get_intervention(iter->first), iter->second, this->instance->get_t_max(iter->first)));
            break;
        }
    }
    if (ret) this->throw_message(ret, "Start time out of intervention's range", "all start times are in correct range");
    return ret;
}

bool ROADEF::Solution::resource_out_of_bounds() {
    bool ret = CORRECT;
    fitness_t_  workload;
    for (uint_t resource : this->instance->get_resources()) {
        for (uint_t time : this->instance->get_horizons()) {
            workload = 0;
            /* sum workload of current resource in current time used on all interventions */
            for (uint_to_uint_t::iterator iter = this->start_times.begin(); iter != this->start_times.end(); ++iter) {
                workload += this->instance->r[this->instance->get_r_index(iter->first, iter->second, resource, time)];
            }
            /* check upper bound */
            if (workload > this->instance->u[this->instance->get_ul_index(resource, time)] + NUMERIC_TOLERANCE) {
                ret = ERROR;
                this->throw_message(ret, "", this->workload_bounds_details(UPPER, this->instance->get_resource(resource), time, workload, this->instance->u[this->instance->get_ul_index(resource, time)]));
                goto eoff; /* avoid another condition to escape nested loops */
            }
            /* check lower bound */
            if (workload < this->instance->l[this->instance->get_ul_index(resource, time)] - NUMERIC_TOLERANCE) {
                ret = ERROR;
                this->throw_message(ret, "", this->workload_bounds_details(LOWER, this->instance->get_resource(resource), time, workload, this->instance->l[this->instance->get_ul_index(resource, time)]));
                goto eoff; /* avoid another condition to escape nested loops */
            }
        }
    }
    eoff:;
    if (ret) this->throw_message(ret, "Resource workload", "all resources are in bounds");
    return ret;
}

bool ROADEF::Solution::unmet_exclusion() {
    bool ret = CORRECT;
    uint_t i1_start;
    uint_t i2_start;
    uint_t i1_delta;
    uint_t i2_delta;
    for (const uint_pair_t &exclusion : this->instance->get_exclusion_pairs()) {
        /* check if interventions from exclusion were already scheduled */
        if (
            this->start_times.find(exclusion.first) == this->start_times.end() ||
            this->start_times.find(exclusion.second) == this->start_times.end()
        ) continue;
        /* load parameters of the two checked interventions */
        i1_start = this->start_times[exclusion.first];
        i2_start = this->start_times[exclusion.second];
        i1_delta = this->instance->delta[this->instance->get_delta_index(exclusion.first, i1_start)];
        i2_delta = this->instance->delta[this->instance->get_delta_index(exclusion.second, i2_start)];
        for (uint_t horizon : this->instance->get_exclusion_season(exclusion.first, exclusion.second)) {
            /* for each time horizon in exclusion check if the interventions are not performed at the same time */
            if (
                horizon >= i1_start &&
                horizon >= i2_start &&
                horizon < i1_start + i1_delta &&
                horizon < i2_start + i2_delta
            ) {
                ret = false;
                this->throw_message(ret, "", this->exclusion_details(this->instance->get_intervention(exclusion.first), this->instance->get_intervention(exclusion.second), horizon));
                goto eoff;
            }
        }
    }
    eoff:;
    if (ret) this->throw_message(ret, "Unmet exclusions", "all exclusions are respected");
    return ret;
}

void ROADEF::Solution::compute_objectives() {
    fitness_t_ mean_risk = 0.0; /* sum of mean risks in all times */
    fitness_t_ mean_risk_in_time; /* help variable to store mean risk in current time */
    fitness_t_ temp; /* help variable to hold values which are used on multiple places to avoid computing the same thing */
    vector<fitness_t_> quantile_in_time; /* holds risks in time to compute quantile */
    quantile_in_time.resize(this->instance->get_max_scenarios());
    fitness_t_ excess; /* variable to hold excess in time t max(0, q_t - mean_risk_t) */
    fitness_t_ total_excess = 0.0; /* sum of quantiles in all times */
    uint_t horizons = this->instance->get_horizon_num(); /* number of time horizons */
    uint_t scenarios_in_time; /* help variable to hold number of scenarios in specific time */
    uint_t index; /* holds index in vector where is located quantile value */
    fitness_t_ q; /* help variable to hold max(0, q_t - mean_risk_t) */
    for (uint_t time = 1; time <= horizons; ++time) {
        mean_risk_in_time = 0.0;
        fill(quantile_in_time.begin(), quantile_in_time.end(), 0.0); /* reset quantiles from previous time */
        scenarios_in_time = this->instance->get_scenarios(time);
        for (uint_pair_t items : this->start_times) { /* item corresponds to pair <intervention, start_time> */
            if (time >= items.second && time < (items.second + this->instance->delta[this->instance->get_delta_index(items.first, items.second)])) {
                for (uint_t scenario = 1; scenario <= scenarios_in_time; ++scenario) {
                    temp = this->instance->risk[this->instance->get_risk_index(items.first, items.second, time)][TO_INDEX(scenario)];
                    mean_risk_in_time += temp;
                    quantile_in_time[scenario - 1] += temp;
                }
            }
        }
        temp = mean_risk_in_time / scenarios_in_time;
        this->mean_cumulative_risks[time] = temp;
        index = ceil(scenarios_in_time * this->instance->get_quantile()) - 1; /* compute position of the quatile in the vector */
        nth_element(quantile_in_time.begin(), quantile_in_time.begin() + index, quantile_in_time.begin() + scenarios_in_time); /* partially sort the vector */
        q = quantile_in_time[index] - temp;
        excess = (q > 0) ? q : 0; /* max(0, q_t - mean_risk_t) */
        total_excess += excess;
        this->expected_excesses[time] = excess;
        mean_risk += temp;
    }
    this->expected_excess = total_excess / horizons;
    this->mean_risk = mean_risk / horizons;

    this->final_objective = this->instance->get_alpha() * this->mean_risk + (1 - this->instance->get_alpha()) * this->expected_excess;
}

void ROADEF::Solution::compute_final_objective() {
    this->final_objective = this->instance->get_alpha() * this->mean_risk + (1 - this->instance->get_alpha()) * this->expected_excess;
}

void ROADEF::Solution::compute_workload_misuse() {
    fitness_t_ workload_ct;
    this->workload_underuse = 0.0;
    this->workload_overuse = 0.0;
    // this->workload_usage = 0.0;
    for (uint_t resource : this->instance->get_resources()) {
        this->resource_overuse[resource] = 0.0;
        this->resource_underuse[resource] = 0.0;
        // this->resource_usage[resource] = 0.0;
        for (uint_t time : this->instance->get_horizons()) {
            workload_ct = 0.0;
            for (uint_to_uint_t::iterator it = this->start_times.begin(); it != this->start_times.end(); ++it) {
                workload_ct += this->instance->r[this->instance->get_r_index(it->first, it->second, resource, time)];
            }
            fitness_t_ max = this->instance->u[this->instance->get_ul_index(resource, time)];
            fitness_t_ min = this->instance->l[this->instance->get_ul_index(resource, time)];
            if (workload_ct > max + NUMERIC_TOLERANCE) {
                workload_ct -= max;
                this->resource_overuse[resource] += workload_ct;
                this->workload_overuse += workload_ct;
            } else if (workload_ct < min - NUMERIC_TOLERANCE) {
                workload_ct = min - workload_ct;
                this->resource_underuse[resource] += workload_ct;
                this->workload_underuse += workload_ct;
            } /* else {
                fitness_t_ tmp = ((workload_ct - min) / (max - min));
                this->resource_usage[resource] += tmp;
                this->workload_usage += tmp;
            } */
        }
    }
}

void ROADEF::Solution::count_unmet_exclusions() {
    fill(this->exclusions_at_time.begin(), this->exclusions_at_time.end(), 0);
    this->exclusion_penalty = 0;
    uint_t i1_start;
    uint_t i2_start;
    uint_t i1_delta;
    uint_t i2_delta;
    for (const uint_pair_t &exclusion : this->instance->get_exclusion_pairs()) {
        /* check if interventions from exclusion were already scheduled */
        if (
            this->start_times.find(exclusion.first) == this->start_times.end() ||
            this->start_times.find(exclusion.second) == this->start_times.end()
        ) continue;
        /* load parameters of the two checked interventions */
        i1_start = this->start_times[exclusion.first];
        i2_start = this->start_times[exclusion.second];
        i1_delta = this->instance->delta[this->instance->get_delta_index(exclusion.first, i1_start)];
        i2_delta = this->instance->delta[this->instance->get_delta_index(exclusion.second, i2_start)];
        for (uint_t horizon : this->instance->get_exclusion_season(exclusion.first, exclusion.second)) {
            /* for each time horizon in exclusion check if the interventions are not performed at the same time */
            if (
                horizon >= i1_start &&
                horizon >= i2_start &&
                horizon < i1_start + i1_delta &&
                horizon < i2_start + i2_delta
            ) {
                ++this->exclusions_at_time[horizon];
                ++this->exclusion_penalty;
            }
        }
    }
}

void ROADEF::Solution::compute_extended_objective() {
    this->extended_objective = this->final_objective + BETA_LOWER * this->workload_underuse + BETA_UPPER  * this->workload_overuse + GAMMA * this->exclusion_penalty;
}

void ROADEF::Solution::generate_random_solution() {
    /* init random generator */
    this->start_times.clear();
    random_device random;
    mt19937 gen(random());
    uniform_int_distribution<> range(1, this->instance->get_horizon_num());
    /* for each intervention insert random start time to solution */
    for (uint_t intervention : this->instance->get_interventions()) {
        this->schedule(intervention, range(gen));
    }
}

void ROADEF::Solution::round_up(int digits) {
    this->workload_overuse = floor(pow(10, digits) * this->workload_overuse + 0.5)/pow(10, digits);
    this->workload_underuse = floor(pow(10, digits) * this->workload_underuse + 0.5)/pow(10, digits);
    this->compute_extended_objective();
}



