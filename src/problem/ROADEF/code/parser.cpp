#include "parser.hpp"

/* constructor with const attr initialization */
Parser::Parser(string path_to_file) : path_to_file(path_to_file) {}

/* tries to open json file and parse its content to j object */
void Parser::load() {
    ifstream input;
    input.open(this->path_to_file);
    if (input.is_open()) {
        input >> this->j;
    } else util::throw_err("Error while opening file");
}

/* extracts data from json object and translates them to instance object */
void Parser::process(ROADEF::Instance *instance) {
     /* extract scenarios */
    for (auto& i : j[SCENARIOS]) {
        instance->add_scenario(i);
    }
    /* create horizons */
    instance->fill_horizons(j[HORIZONS]);
    /* load evaluation parameters */
    instance->set_eval_parameters(
        j[QUANTILE],
        j[ALPHA],
        j[COMP_TIME]
    );
    /* map resources */
    uint_t map_key;
    instance->prepare_products(j[INTERVENTIONS].size(), j[RESOURCES].size(), j[HORIZONS]);
    instance->allocate_arrays();
    uint_t idx;
    for (auto& i : j[RESOURCES].items()) {
        map_key = instance->add_resource(i.key());
        /* store maximum and mininimum for each resource in time horizons */
        uint_t count = 1;
        idx = instance->get_ul_index(map_key, 1);
        for (fitness_t_ max : i.value()[MAX]) {
            instance->u[idx++] = max;
        }
        idx = instance->get_ul_index(map_key, 1);
        for (fitness_t_ min : i.value()[MIN]) {
            instance->l[idx++] = min;
        }
    }
    /* map interventions */
    uint_t t_max;
    for (auto& i : j[INTERVENTIONS].items()) {
        /* translate name into unique key */
        map_key = instance->add_intervention(i.key());
        /* store intervnetion's latest start time horizon */
        t_max = util::convert_to_int(i.value()[T_MAX]);
        instance->add_t_max(map_key, t_max);
        /* store deltas */
        uint_t horizon = 1;
        idx = instance->get_delta_index(map_key, 1);
        for (uint_t delta : i.value()[DELTA]) {
            instance->delta[idx++] = delta;
        }
        uint_t t_int;
        uint_t idx;
        uint_t resource_id;
        /* store resources needed for each intervention in timehorizon based on start time */
        for (auto &resource : i.value()[WORKLOAD].items()) {
            resource_id = instance->get_resource_id(resource.key());
            for (auto &time : resource.value().items()) {
                t_int = util::convert_to_int(time.key());
                for (auto &start_time : time.value().items()) {
                    idx = instance->get_r_index(map_key, util::convert_to_int(start_time.key()), resource_id, t_int);
                    instance->add_r(map_key, resource_id, idx, start_time.value());
                }
            }
        }
        /* store risks (cost) of each intervention in time and scenario based on start time */
        for (auto &time : i.value()[RISK].items()) {
            t_int = util::convert_to_int(time.key());
            for (auto &start_time : time.value().items()) {
                for (uint_t scenario = 0; scenario < instance->get_scenarios(t_int); ++scenario) {
                    instance->risk[instance->get_risk_index(map_key, util::convert_to_int(start_time.key()), t_int)].push_back((half_fitness_t_)start_time.value()[scenario]);
                }
            }
        }
    }
    this->process_exclusions(instance);
    this->process_avg_properties(instance);
}

/* extracts exclusions from json and maps pair of instances to row index in season table */
void Parser::process_exclusions(ROADEF::Instance *instance) {
    /* fill season table with basic seasons (first line is emtpy, will be popped) */
    unordered_map<string, uint_t> seas_names;
    instance->append_season();
    uint_t count = 1;
    for (auto &season : j[SEASONS].items()) {
        seas_names[season.key()] = count;
        instance->append_season();
        for (auto &value : season.value()) {
            instance->append_to_season(count, util::convert_to_int(value));
        }
        count++;
    }
    vector<tuple<uint_t, uint_t, vector<string>>> filtered_exclusions;
    uint_t id1;
    uint_t id2;
    bool unique;
    for (auto &exclusion : j[EXCLUSIONS]) {
        /* translate interventions into ids */
        id1 = instance->get_intervention_id(exclusion[0]);
        id2 = instance->get_intervention_id(exclusion[1]);
        /* swap ids if not in ascending order */
        if (id1 > id2) {
            uint_t temp = id1;
            id1 = id2;
            id2 = temp;
        }
        /* filter exclusions so each pair is there only once -> join their season parameters */
        unique = true;
        for (tuple<uint_t, uint_t, vector<string>> &trinity : filtered_exclusions) {
            if (id1 == get<0>(trinity) && id2 == get<1>(trinity)) {
                unique = false;
                get<2>(trinity).push_back(exclusion[2]);
                break;
            }
        }
        if (unique) filtered_exclusions.push_back(make_tuple(id1, id2, vector<string>{exclusion[2]}));
    }
    /* maps each pair to uniqe row in season table which contains all of its seasons combined into one row */
    for (tuple<uint_t, uint_t, vector<string>> &trinity : filtered_exclusions) {
        string season_comb = util::combine_strings(get<2>(trinity));
        unordered_map<string, uint_t>::iterator element = seas_names.find(season_comb);
        /* if season combination exists, map the exclusion to it */
        if (element != seas_names.end()) {
            instance->add_exclusion(get<0>(trinity), get<1>(trinity), element->second);
        } else {
            /* if it is new, add it to season table and map the exclusion to it */
            instance->append_season();
            for (string s : get<2>(trinity)) {
                instance->combine_seasons(count, seas_names[s]);
            }
            instance->sort_season(count);
            instance->add_exclusion(get<0>(trinity), get<1>(trinity), count);
            seas_names[season_comb] = count++;
        }
    }
}

bool cmp_pair(pair<uint_t, fitness_t_>& a,
              pair<uint_t, fitness_t_>& b)
{
    return a.second < b.second;
}

void Parser::process_avg_properties(ROADEF::Instance *instance) {
    vector<pair<uint_t, fitness_t_>> avg_deltas;
    vector<pair<uint_t, fitness_t_>> avg_costs;
    vector<pair<uint_t, fitness_t_>> avg_rds;

    std::default_random_engine engine(1);
    ROADEF::Solution s(instance, &engine);

    for (auto i:instance->get_interventions()) {
        // cout << "i: " << i << endl;
        auto delta_idx = instance->get_delta_index(i, 1);
        auto t_max = instance->get_t_max(i);

        fitness_t_ delta_avg = 0;
        fitness_t_ cost_avg = 0;
        fitness_t_ rd_avg = 0;

        for (uint_t t = 1; t <= t_max; ++t) {
            auto delta = instance->delta[delta_idx++];
            auto obj = s.estimate_schedule(i, t);
            auto cost = obj.final_objective;
            auto rd = obj.total_resource_use;
            // cout << "t: " << t << ", delta: " << delta << ", cost: " << cost << ", rd: " << rd << endl;

            delta_avg += delta;
            cost_avg += cost;
            rd_avg += rd;
        }

        delta_avg /= t_max;
        cost_avg /= t_max;
        rd_avg /= t_max;

        avg_deltas.emplace_back(i, delta_avg);
        avg_costs.emplace_back(i, cost_avg);
        avg_rds.emplace_back(i, rd_avg);
    }

    sort(avg_deltas.begin(), avg_deltas.end(), cmp_pair);
    sort(avg_costs.begin(), avg_costs.end(), cmp_pair);
    sort(avg_rds.begin(), avg_rds.end(), cmp_pair);

    instance->set_avg_deltas(avg_deltas);
    instance->set_avg_costs(avg_costs);
    instance->set_avg_rds(avg_rds);
}