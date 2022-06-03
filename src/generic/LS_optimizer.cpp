#include "LS_optimizer.hpp"

#include <utility>

//**********************************************************************
// INITIALIZATION
//**********************************************************************

LS_optimizer::LS_optimizer(Instance *inst, json config, uint seed) {
    this->instance = inst;
    this->config = std::move(config);
    Solution sol(inst->node_cnt);
    this->solution = sol;
    this->best_known_solution = sol;
    this->rng = LS_optimizer::init_rng(seed);
    this->timeout_s = this->config["timeout"].get<uint>();
    this->setConstruction(this->config["construction"].get<string>());
    this->setMetaheuristic(this->config["metaheuristic"].get<string>());
    this->setLocalSearch(this->config["local_search"].get<string>());
    this->setPerturbations(this->config["perturbation"].get<vector<string>>());
    this->setOperators(this->config["operators"].get<vector<string>>());
    omp_set_num_threads(this->config["num_threads"]);
}

std::mt19937* LS_optimizer::init_rng(uint seed) {
    if (seed == 0) {
        std::random_device rd;     // only used once to initialise (seed) engine
        seed = rd();
    }
    return new std::mt19937(seed);

}

void LS_optimizer::setConstruction(const string& constr) {
    if (constr == "greedy")
        this->construction = [this] { construct_greedy(); };
    else if (constr == "random")
        this->construction = [this] { construct_random(); };
    else if (constr == "random_replicate")
        this->construction = [this] { construct_random_replicate(); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void LS_optimizer::setMetaheuristic(const string& meta) {
    if (meta == "ILS")
        this->metaheuristic = [this] { ILS(); };
    else if (meta == "BVNS")
        this->metaheuristic = [this] { basicVNS(); };
    else if (meta == "CVNS")
        this->metaheuristic = [this] { calibratedVNS(); };
    else
        throw std::system_error(EINVAL, std::system_category(), meta);
}

void LS_optimizer::setLocalSearch(const string& vnd_method) {
    if (vnd_method == "BVND")
        this->local_search = [this] { basicVND(); };
    else if (vnd_method == "PVND")
        this->local_search = [this] { pipeVND(); };
    else if (vnd_method == "CVND")
        this->local_search = [this] { cyclicVND(); };
    else if (vnd_method == "RVND")
        this->local_search = [this] { randomVND(); };
    else if (vnd_method == "RPVND")
        this->local_search = [this] { randompipeVND(); };
    else
        throw std::system_error(EINVAL, std::system_category(), vnd_method);
}

void LS_optimizer::setOperators(const vector<string>& operators) {
    this->operation_list = operators;
    if (this->operation_list.empty())
        throw std::system_error(EINVAL, std::system_category(), "ERROR: No neighborhood selected!");
    for (const auto& op_name : operators) {
        this->operation_histogram[op_name] = std::pair<uint, uint>(0, 0);
    }
}

void LS_optimizer::setPerturbations(const vector<string>& perturbations) {
    this->perturbation_list = perturbations;
    if (this->perturbation_list.empty())
        throw std::system_error(EINVAL, std::system_category(), "ERROR: No perturbation selected!");
}

void LS_optimizer::setLogger(std::basic_ostream<char> &logs) {
    if (this->log_stream == nullptr) {
        this->log_stream = &logs;
    } else {
        std::cout << "WARNING: Logger already set\n";
    }
}

void LS_optimizer::run() {
    this->start = std::chrono::steady_clock::now();
    this->last_improvement = this->start;
    this->construction();
    this->metaheuristic();
}

//**********************************************************************
// OPERATORS
//**********************************************************************

bool LS_optimizer::insert1() {
    Solution best_solution(this->instance->node_cnt, this->solution.frequency);
    vector<uint> perm = this->solution.permutation;
    fitness_t fitness;
    uint inserted_node = 0;
    bool updated = false;
#pragma omp parallel for default(none) private(fitness) shared(best_solution, inserted_node, perm)
    for (uint i = 0; i <= perm.size(); i++) {
        vector<uint> new_perm = perm;
        new_perm.insert(new_perm.begin() + i, 0);
        for (uint j = 0; j < this->instance->node_cnt; j++){
            if (this->solution.frequency[j] >= this->instance->ubs[j]) {continue;}
            new_perm[i] = j;
            this->instance->compute_fitness(new_perm, &fitness);
#pragma omp critical
             if (fitness < best_solution.fitness) {
                best_solution = make_solution(new_perm);
                inserted_node = j;
            }
        }
    }
    if (best_solution < this->solution) {
        updated = true;
        this->solution.copy(best_solution);
        this->solution.frequency[inserted_node]++; // increment frequenc
    }
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(updated, str(format("%1%") % __func__));
#endif
    return updated;
}

bool LS_optimizer::remove1() {
    if (this->solution.permutation.empty())
        return false;
    vector<uint> perm = this->solution.permutation;
    uint removed_node = 0;
    fitness_t fitness;
    Solution best_solution(this->instance->node_cnt, this->solution.frequency);
    bool updated = false;
#pragma omp parallel for default(none) private(fitness) shared(best_solution, removed_node, perm)
    for (uint i = 0; i < perm.size(); i++){
        if (this->solution.frequency[perm[i]] <= this->instance->lbs[perm[i]]) {continue;}
        vector<uint> new_perm = perm;
        new_perm.erase(new_perm.begin() + i);
        this->instance->compute_fitness(new_perm, &fitness);
#pragma omp critical
        if (fitness < best_solution.fitness) {
            best_solution = make_solution(new_perm);
            removed_node = perm[i];
        }
    }
    if (best_solution <= this->solution) {
        updated = true;
        this->solution.copy(best_solution);
        this->solution.frequency[removed_node]--; // decrement frequency
    }
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(updated, str(format("%1%") % __func__));
#endif
    return updated;
}

bool LS_optimizer::relocate(uint x, bool reverse) {
    if (x > this->solution.permutation.size())
        return false;
    fitness_t fitness;
    vector<uint> perm = this->solution.permutation;
    Solution best_solution(this->instance->node_cnt, this->solution.frequency);
    bool updated = false;
#pragma omp parallel for default(none) private(fitness) shared(best_solution, perm, x, reverse)
    for (uint i = 0; i < perm.size() - x; i++) {
        vector<uint> reduced_perm = perm;
        vector<uint> subsequence(perm.begin() + i, perm.begin() + i + x);
        reduced_perm.erase(reduced_perm.begin() + i, reduced_perm.begin() + i + x);
        for (uint j = 0; j < reduced_perm.size(); j++) {
            vector<uint> new_perm = reduced_perm;
            if (reverse) {
                new_perm.insert(new_perm.begin() + j, subsequence.rbegin(), subsequence.rend());
            } else {
                new_perm.insert(new_perm.begin() + j, subsequence.begin(), subsequence.end());
            }
            this->instance->compute_fitness(new_perm, &fitness);
#pragma omp critical
            if (fitness < best_solution.fitness) {
                best_solution = make_solution(new_perm);
            }
        }
    }
    if (best_solution < this->solution) {
        updated = true;
        this->solution.copy(best_solution);
    }
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(updated, str(format("%1%%2%_%3%")
                                       % (reverse ? 'r' : ' ') % __func__ % x ));
#endif
    return updated;
}

bool LS_optimizer::centered_exchange(uint x) {
    fitness_t fitness;
    if (x > this->solution.permutation.size())
        return false;
    vector<uint> perm = this->solution.permutation;
    Solution best_solution(this->instance->node_cnt, this->solution.frequency);
    bool updated = false;
#pragma omp parallel for default(none) private(fitness) shared(best_solution, perm, x)
    for (int i = (int)x; i < (int)perm.size() - (int)x; i++) {
        vector<uint> new_perm = perm;
        for (int j = 1; j <= (int)x; j++) {
            new_perm[i+j] = perm[i-j];
            new_perm[i-j] = perm[i+j];
        }
        this->instance->compute_fitness(new_perm, &fitness);
#pragma omp critical
        if (fitness < best_solution.fitness) {
            best_solution = make_solution(new_perm);
        }
    }
    if (best_solution < this->solution) {
        updated = true;
        this->solution.copy(best_solution);
    }
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(updated, str(format("%1%_%2%") % __func__ % x));
#endif
    return updated;
}

bool LS_optimizer::exchange(uint x, uint y, bool reverse) {
    if (x + y > this->solution.permutation.size())
        return false;
    fitness_t fitness;
    vector<uint> perm = this->solution.permutation;
    Solution best_solution(this->instance->node_cnt, this->solution.frequency);
    bool updated = false;
#pragma omp parallel for default(none) private(fitness) shared(best_solution, perm, x, y, reverse)
    for (uint i = 0; i < perm.size() - x; i++) {
        for (uint j = 0; j < perm.size() - y; j++) {
            if ((i >= j && i < j + y) || (j >= i && j < i + x)) {
                continue;
            }
            vector<uint> subx, suby, new_perm = perm;
            if (reverse) {
                subx.insert(subx.begin(), perm.rend() - i - x, perm.rend() - i);
                suby.insert(suby.begin(), perm.rend() - j - y, perm.rend() - j);
            } else {
                subx.insert(subx.begin(), perm.begin() + i, perm.begin() + i + x);
                suby.insert(suby.begin(), perm.begin() + j, perm.begin() + j + y);
            }
            if (i < j) {
                new_perm.erase(new_perm.begin() + j, new_perm.begin() + j + y);
                new_perm.insert(new_perm.begin() + j, subx.begin(), subx.end());
                new_perm.erase(new_perm.begin() + i, new_perm.begin() + i + x);
                new_perm.insert(new_perm.begin() + i, suby.begin(), suby.end());
            } else {
                new_perm.erase(new_perm.begin() + i, new_perm.begin() + i + x);
                new_perm.insert(new_perm.begin() + i, suby.begin(), suby.end());
                new_perm.erase(new_perm.begin() + j, new_perm.begin() + j + y);
                new_perm.insert(new_perm.begin() + j, subx.begin(), subx.end());
            }
            this->instance->compute_fitness(new_perm, &fitness);
#pragma omp critical
            if (fitness < best_solution.fitness) {
                best_solution = make_solution(new_perm);
            }
        }
    }
    if (best_solution < this->solution) {
        updated = true;
        this->solution.copy(best_solution);
    }
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(updated, str(format("%1%%2%_%3%_%4%")
        % (reverse ? 'r' : ' ')  % __func__ % x % y));
#endif
    return updated;
}

bool LS_optimizer::move_all(uint x) {
    if (this->solution.permutation.size() < 2)
        return false;
    vector<uint> perm = this->solution.permutation;
    fitness_t fitness;
    Solution best_solution(this->instance->node_cnt, this->solution.frequency);
    bool updated = false;
#pragma omp parallel for default(none) private(fitness) shared(best_solution, perm, x)
    for (uint node_id = 0; node_id < this->instance->node_cnt; node_id++){
        vector<uint> positions;
        for (uint i = 0; i < perm.size(); i++){
            if (perm[i] == node_id){
                positions.push_back(i);
            }
        }
        for (int i = -(int)x; i <= (int)x; i++){
            vector<uint> new_perm = perm;
            if (i == 0) continue;
            for (auto pos : positions){
                int new_pos = (int)pos + i;
                if (new_pos < 0)
                    new_pos = (int)new_perm.size() + new_pos;
                else if ((uint)new_pos > new_perm.size() - 1)
                    new_pos = new_pos - (int)new_perm.size();
                new_perm.erase(new_perm.begin() + pos);
                new_perm.insert(new_perm.begin() + new_pos, node_id);
            }
            this->instance->compute_fitness(new_perm, &fitness);
#pragma omp critical
            if (fitness < best_solution.fitness) {
                best_solution = make_solution(new_perm);
            }
        }
    }
    if (best_solution < this->solution) {
        updated = true;
        this->solution.copy(best_solution);
    }
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(updated, str(format("%1%_%2%") % __func__ % x));
#endif
    return updated;
}

bool LS_optimizer::exchange_ids() {
    if (this->solution.permutation.size() < 2)
        return false;
    vector<uint> perm = this->solution.permutation;
    uint id1, id2, cnt_id1=0, cnt_id2=0;
    fitness_t fitness;
    Solution best_solution(this->instance->node_cnt, this->solution.frequency);
    bool updated = false;
#pragma omp parallel for default(none) private(fitness) shared(best_solution, id1, cnt_id1, id2, cnt_id2, perm)
    for (uint i = 0; i < this->instance->node_cnt; i++) {
        for (uint j = 0; j < i; j++) {
            uint counter1 = 0, counter2 = 0;
            vector<uint> new_perm = perm;
            for (unsigned int & node : new_perm){
                if (node == i) {
                    counter1++;
                    node = j;
                } else if (node == j) {
                    counter2++;
                    node = i;
                }
            }
            this->instance->compute_fitness(new_perm, &fitness);
#pragma omp critical
            if (fitness < best_solution.fitness) {
                best_solution = make_solution(new_perm);
                id1 = i; id2 = j;
                cnt_id1 = counter1; cnt_id2 = counter2;
            }
        }
    }
    if (best_solution < this->solution) {
        updated = true;
        this->solution.copy(best_solution);
        this->solution.frequency[id1] += cnt_id2 - cnt_id1;
        this->solution.frequency[id2] += cnt_id1 - cnt_id2;
    }
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(updated, str(format("%1%") % __func__));
#endif
    return updated;
}

bool LS_optimizer::exchange_n_ids() {
    if (this->solution.permutation.size() < 2)
        return false;
    vector<uint> perm = this->solution.permutation;
    uint split, id1, id2, cnt_id1=0, cnt_id2=0;
    fitness_t fitness;
    Solution best_solution(this->instance->node_cnt, this->solution.frequency);
    bool updated = false;
#pragma omp parallel for default(none) private(fitness) shared(best_solution, split, id1, cnt_id1, id2, cnt_id2, perm)
    for (uint i = 0; i < this->instance->node_cnt; i++) {
        for (uint j = 0; j < i; j++) {
            for (uint n = 1; n < this->solution.frequency[i]; n++) {
                uint counter1 = 0, counter2 = 0;
                vector<uint> new_perm = perm;
                for (unsigned int &node: new_perm) {
                    if (node == i && counter1 < n) {
                        counter1++;
                        node = j;
                    } else if (node == j && counter2 < n) {
                        counter2++;
                        node = i;
                    }
                }
                this->instance->compute_fitness(new_perm, &fitness);
#pragma omp critical
                if (fitness < best_solution.fitness) {
                    best_solution = make_solution(new_perm);
                    id1 = i; id2 = j;
                    split = n;
                    cnt_id1 = counter1; cnt_id2 = counter2;
                }
            }
        }
    }
    if (best_solution < this->solution) {
        updated = true;
        this->solution.copy(best_solution);
        this->solution.frequency[id1] += cnt_id2 - cnt_id1;
        this->solution.frequency[id2] += cnt_id1 - cnt_id2;
    }
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(updated, str(format("%1% %2%") % __func__ % split));
#endif
    return updated;
}

bool LS_optimizer::two_opt() {
    fitness_t fitness;
    vector<uint> perm = this->solution.permutation;
    if (perm.size() < 2)
        return false;
    Solution best_solution(this->instance->node_cnt, this->solution.frequency);
    bool updated = false;
#pragma omp parallel for default(none) private(fitness) shared(best_solution, perm)
    for (uint i = 0; i <= perm.size() - 2; i++) {
        for (uint j = i+2; j <= perm.size(); j++) {
            vector<uint> new_perm = perm;
            reverse(new_perm.begin() + i, new_perm.begin() + j); // half closed interval [i, j)
            this->instance->compute_fitness(new_perm, &fitness);
#pragma omp critical
            if (fitness < best_solution.fitness) {
                best_solution = make_solution(new_perm);
            }
        }
    }
    if (best_solution < this->solution) {
        updated = true;
        this->solution.copy(best_solution);
    }
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(updated, str(format("%1%") % __func__));
#endif
    return updated;
}

// **********************************************************************
// PERTURBATIONS
// **********************************************************************

void LS_optimizer::double_bridge(uint k, bool reverse_all) {
    if (k < 1) throw std::out_of_range("Double bridge: k < 1");
    string log_args[k];
    std::uniform_int_distribution<uint> uni(0, this->instance->node_cnt-1);
    std::vector<uint> idx;
    vector<uint> perm = this->solution.permutation;
    Solution new_solution(this->instance->node_cnt, this->solution.frequency);
    vector<uint> new_perm = perm;
    // generate random sorted indices
    for (auto & i : log_args) {
        idx.push_back(uni(*rng));
        i = uitos(idx.back());
    }
    std::sort(std::begin(idx), std::end(idx));
    // reverse subpermutations
    if (!reverse_all) {
        this->random_reverse(new_perm.begin(), new_perm.begin() + idx[0]); // half closed interval [i, j)
        for (uint i = 0; i + 1 < k; i++)
            this->random_reverse(new_perm.begin() + idx[i],
                                 new_perm.begin() + idx[i + 1]); // half closed interval [i, j)
        this->random_reverse(new_perm.begin() + idx[k - 1], new_perm.end()); // half closed interval [i, j)
    } else {
        reverse(new_perm.begin(), new_perm.begin() + idx[0]); // half closed interval [i, j)
        for (uint i = 0; i + 1 < k; i++)
            reverse(new_perm.begin() + idx[i],
                                 new_perm.begin() + idx[i + 1]); // half closed interval [i, j)
        reverse(new_perm.begin() + idx[k - 1], new_perm.end()); // half closed interval [i, j)
    }
    // copy new solution
    new_solution.permutation = new_perm;
    new_solution.is_feasible = this->instance->compute_fitness(new_perm, &new_solution.fitness);
    this->solution.copy(new_solution);
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(true, str(format("%1% %2%") % __func__ % k));
#endif
}

void LS_optimizer::random_swap(uint k) {
    uint sel1, sel2, temp;
    vector<uint> perm = this->solution.permutation;
    std::uniform_int_distribution<uint> uni_select(0, perm.size() - 1);
    for (uint i = 0; i < k; i++) {
        sel1 = uni_select(*this->rng);
        sel2 = uni_select(*this->rng);
        while (sel1 == sel2) { sel2 = uni_select(*this->rng); }
        temp = perm[sel1];
        perm[sel1] = perm[sel2];
        perm[sel2] = temp;
    }
    this->solution = make_solution(perm);
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(true, str(format("%1% %2%") % __func__ % k));
#endif
}

void LS_optimizer::random_move(uint k) {
    uint sel1, sel2, temp;
    vector<uint> perm = this->solution.permutation;
    std::uniform_int_distribution<uint> uni_select(0, perm.size() - 1);
    for (uint i = 0; i < k; i++) {
        sel1 = uni_select(*this->rng);
        sel2 = uni_select(*this->rng);
        while (sel1 == sel2) { sel2 = uni_select(*this->rng); }
        temp = perm[sel1];
        perm.erase(perm.begin() + sel1);
        perm.insert(perm.begin() + sel2, temp);
    }
    this->solution = make_solution(perm);
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(true, str(format("%1% %2%") % __func__ % k));
#endif
}

void LS_optimizer::reinsert(uint k) {
    vector<uint> idx_choice, node_cnt;
    vector<string> log_args;
    vector<uint> perm = this->solution.permutation;
    // get nodes randomly
    std::uniform_int_distribution<uint> uni_select(0, this->instance->node_cnt-1);
    for (uint i = 0; i < k; i++) {
        idx_choice.push_back(uni_select(*this->rng));
        log_args.push_back(uitos(idx_choice.back()));
        node_cnt.push_back(0);
    }
    // remove all selected nodes
    for (uint j = 0; j < k; j++) {
        for (uint i = 0; i < perm.size(); i++) {
            if (idx_choice[j] == perm[i]) {
                perm.erase(perm.begin() + i);
                node_cnt[j]++;
            }
        }
    }
    // insert selected nodes randomly
    uint node_id;
    for (uint j = 0; j < node_cnt.size(); j++) {
        for (uint i = 0; i < node_cnt[j]; i++) {
            node_id = idx_choice[j];
            std::uniform_int_distribution<uint> uni(0, perm.size() - 1);
            perm.insert(perm.begin() + uni(*this->rng), node_id);
        }
    }
    this->solution = make_solution(perm);
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(true, str(format("%1% %2%") % __func__ % k));
#endif
}

void LS_optimizer::random_move_all(uint k) {
    vector<string> log_args;
    int move;
    uint node_id;
    vector<uint> perm = this->solution.permutation;
    vector<uint> positions;
    std::uniform_int_distribution<uint> node_uni(0, this->instance->node_cnt - 1);
    std::uniform_int_distribution<int> dist_uni(-(int)k, (int)k);
    for (uint i = 0; i < k; i++) {
        move = dist_uni(*this->rng);
        while (move == 0)
            move = dist_uni(*this->rng);
        node_id = node_uni(*this->rng);
        positions.clear();
        for (uint j = 0; j < this->solution.permutation.size(); j++) {
            if (this->solution.permutation[j] == node_id) {
                positions.push_back(j);
            }
        }
        std::uniform_int_distribution<uint> split_uni(0, positions.size());
        uint split = split_uni(*this->rng);
        uint counter = 0;
        for (auto pos: positions) {
            int new_pos = (int) pos + (counter >= split ? move : 0);
            if (new_pos < 0)
                new_pos = (int) perm.size() + new_pos;
            else if ((uint) new_pos > perm.size() - 1)
                new_pos = new_pos - (int) perm.size();
            perm.erase(perm.begin() + pos);
            perm.insert(perm.begin() + new_pos, node_id);
            counter++;
        }
    }
    this->solution = make_solution(perm);
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->print_operation(true, str(format("%1% %2%") % __func__ % k));
#endif
}

//**********************************************************************
// VARIABLE NEIGHBORHOOD DESCENT
//**********************************************************************

void LS_optimizer::basicVND() {
    std::chrono::steady_clock::time_point now;
    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max();
    fitness_t current_fitness = prev_fitness - 1;
    while (current_fitness < prev_fitness) {
        for (const auto &operation : this->operation_list) {
            if (this->timeout()) return;
            if (this->operation_call(operation)) { break; }
        }
        prev_fitness = current_fitness;
        current_fitness = this->solution.fitness;
    }
}

void LS_optimizer::pipeVND() {
    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max();
    fitness_t current_fitness = prev_fitness - 1;
    while (current_fitness < prev_fitness) {
        int last_improving_operator = -1;
        for (int i = 0; i < (int)this->operation_list.size(); i++) {
            if (last_improving_operator == i) {return;}
            string operation = this->operation_list[i];
            while (this->operation_call(operation)) {
                last_improving_operator = i;
                if (this->timeout()) {return;}
            }
        }
        prev_fitness = current_fitness;
        current_fitness = this->solution.fitness;
    }
}

void LS_optimizer::cyclicVND() {
    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max();
    fitness_t current_fitness = prev_fitness - 1;
    while (current_fitness < prev_fitness) {
        for (const auto &operation : this->operation_list) {
            this->operation_call(operation);
            if (this->timeout()) {return;}
        }
        prev_fitness = current_fitness;
        current_fitness = this->solution.fitness;
    }
}

void LS_optimizer::randomVND() {
    std::uniform_int_distribution<uint> uni(0, this->operation_list.size() - 1);
    vector<uint> order;
    for (uint i = 0; i < operation_list.size(); i++)
        order.push_back(i);
    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max();
    fitness_t current_fitness = prev_fitness - 1;
    while (current_fitness < prev_fitness) {
        std::shuffle(order.begin(), order.end(), *this->rng);
        for (uint idx : order) {
            this->operation_call(this->operation_list[idx]);
            if (this->timeout()) {return;}
        }
        prev_fitness = current_fitness;
        current_fitness = this->solution.fitness;
    }
}

void LS_optimizer::randompipeVND() {
    std::uniform_int_distribution<uint> uni(0, this->operation_list.size() - 1);
    vector<uint> order;
    for (uint i = 0; i < operation_list.size(); i++)
        order.push_back(i);
    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max();
    fitness_t current_fitness = prev_fitness - 1;
    while (current_fitness < prev_fitness) {
        std::shuffle(order.begin(), order.end(), *this->rng);
        for (uint idx : order) {
            while (this->operation_call(this->operation_list[idx])) {
                if (this->timeout()) { return; }
            }
        }
        prev_fitness = current_fitness;
        current_fitness = this->solution.fitness;
    }
}

//**********************************************************************************************************************
// METAHEURISTICS
//**********************************************************************************************************************

/*
 * Iterated Local Search metaheuristic.
 * Alternates pertubation and local search.
 * Parameters:
 * ils_k . . . perturbation parameter
 */
void LS_optimizer::ILS() {
    this->local_search();
    do {
        // Apply perturbation to this->solution
        for (const auto &pert : this->perturbation_list) {
            this->perturbation_call(pert, config["ils_k"].get<uint>());
        }
        // Perform local search on this->solution, ev. update this->best_known_solution
        this->local_search();
        // Reset this->solution to this->best_known_solution
        if (this->best_known_solution.fitness != this->solution.fitness) {
            this->solution.copy(this->best_known_solution);
        }
    } while (!this->timeout());
    std::cout << str(format("%1% Timeout: %2% (sec)") % __func__ % this->timeout_s) << std::endl;
}

/*
 * Basic Variable Neighborhood Search metaheuristic.
 * Alternates perturbation and local search.
 * Perturbation parameter k resets after improving local search, otherwise increases by 1.
 * Parameters:
 * bvns_min_k  . . . initial value of k
 * bvns_max_k . . . max value of k
 */
void LS_optimizer::basicVNS() {
    uint min_k = config["bvns_min_k"].get<uint>();
    uint max_k = config["bvns_max_k"].get<uint>();
    uint k = min_k;

    this->local_search();
    Solution current_best_solution(this->solution); // needed, as this->best_known_solution is updated internally
    do {
        // Apply perturbation to this->solution
        for (const auto &pert : this->perturbation_list) {
            this->perturbation_call(pert, k);
        }
        // Perform local search on this->solution, ev. update this->best_known_solution
        this->local_search();
        // Adjust k
        if (this->solution < current_best_solution){
            current_best_solution.copy(this->solution);
            k = min_k;
        } else {
            k = k < max_k ? k + 1 : max_k;
        }
        // TODO this should never happen
        if (this->best_known_solution.fitness != current_best_solution.fitness)
            std::cerr << str(format("ERROR: %1% bks != best_solution") % __func__ ) << std::endl;
        // Reset this->solution to this->best_known_solution
        this->solution.copy(this->best_known_solution);
    } while (!this->timeout());
    std::cout << str(format("%1% Timeout: %2% (sec)") % __func__ % this->timeout_s) << std::endl;
}

/*
 * Calibrated Variable Neighborhood Search metaheuristic (custom).
 * TODO think through intended logic.
 * Parameters:
 * cvns_min_k . . . initial value of k
 * cvns_max_k . . . max value of k
 * cvns_it_per_k . . . ???
 */
void LS_optimizer::calibratedVNS() {
    uint min_k = config["cvns_min_k"].get<uint>();
    uint max_k = config["cvns_max_k"].get<uint>();
    uint it_per_k = config["cvns_it_per_k"].get<uint>();
    uint same_sol_cnt = 0;
    uint k = min_k;
    this->local_search();

    Solution current_best_solution(this->solution);
    do {
        // Apply perturbation to this->solution
        for (const auto &pert: this->perturbation_list) {
            this->perturbation_call(pert, k);
        }
        // Perform local search on this->solution, ev. update this->best_known_solution
        this->local_search();

        if (this->solution < current_best_solution) { // Improving solution -> reset k
            current_best_solution.copy(this->solution);
            k = min_k;
        }
        if (this->solution.fitness == current_best_solution.fitness){ // Improving or same solution found
            if (++same_sol_cnt == it_per_k) { // Increase k after it_per_k
                same_sol_cnt = 0;
                k = k < max_k ? k + 1 : max_k;
            }
        } else { // Worse solution found
            if (same_sol_cnt > 0) {
                same_sol_cnt--;
            } else {
                //k = k > min_k ? k - 1 : min_k;
            }
        }
        // TODO this should never happen
        if (this->best_known_solution.fitness != current_best_solution.fitness)
            std::cerr << str(format("ERROR: %1% bks != best_solution") % __func__ ) << std::endl;
        // Reset this->solution to this->best_known_solution
        this->solution.copy(this->best_known_solution);
    } while (!this->timeout());
    std::cout << str(format("%1% Timeout: %2% (sec)") % __func__ % this->timeout_s) << std::endl;
}

//**********************************************************************************************************************
// CONSTRUCTIONS
//**********************************************************************************************************************

/*
 * Applies insert1 operator until the solution is not valid w.r.t. LBs and UBs or the operator does not update the solution.
 * Updates existing this->solution.
 */
void LS_optimizer::construct_greedy() {
    bool updated, valid;
    do {
        updated = insert1();
        valid = this->valid_solution(&this->solution);
    } while((!valid || updated) && !this->timeout());
}

/*
 * Adds nodes to random positions of the solution, until all nodes are at their LBs.
 * Updates existing this->solution.
 */
void LS_optimizer::construct_random() {
    uint rnd_idx;
    for (uint node_id = 0; node_id < this->instance->node_cnt; node_id++) {
        for (uint counter = 0; counter < this->instance->lbs[node_id]; counter++) {
            std::uniform_int_distribution<uint> uni(0, this->solution.getSize());
            rnd_idx = uni(*this->rng);
            this->solution.permutation.insert(this->solution.permutation.begin() + rnd_idx, node_id);
        }
    }
}

/*
 * Creates a random permutation of nodes.
 * Replicates the permutation, until the solution is not valid w.r.t. LBs and UBs.
 * Updates existing this->solution.
 */
void LS_optimizer::construct_random_replicate() {
    vector<uint> perm;
    for (uint i = 0; i < this->instance->node_cnt; i++)
        perm.push_back(i);
    std::shuffle(perm.begin(), perm.end(), *this->rng);
    do {
        for (auto elem : perm){
            if (this->solution.frequency[elem] < this->instance->lbs[elem]) {
                this->solution.frequency[elem]++;
                this->solution.permutation.push_back(elem);
            }
        }
    } while(!this->valid_solution(&this->solution) && !this->timeout());
}

//**********************************************************************
// UTILS
//**********************************************************************

bool LS_optimizer::operation_call(const string &operation_name) {
    bool result;
    this->operation_histogram[operation_name].first++;
    result = this->operation_map.at(operation_name)();
    if (result)
        this->operation_histogram[operation_name].second++;
    if (this->solution < this->best_known_solution) {
        this->best_known_solution.copy(this->solution);
        this->last_improvement = std::chrono::steady_clock::now();
        this->steps.emplace_back(this->get_runtime(), this->solution.fitness);
    }
    return result;
}

void LS_optimizer::perturbation_call(const string &perturbation_name, uint k) {
    if (!config["allow_infeasible"].get<bool>()) {
        do {
            this->solution.copy(this->best_known_solution);
            this->perturbation_map.at(perturbation_name)(k); // call perturbation
        } while (this->solution.fitness == this->best_known_solution.fitness || !this->solution.is_feasible);
    } else {
        this->perturbation_map.at(perturbation_name)(k); // call perturbation
    }
}

long LS_optimizer::get_runtime() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - this->start).count();
}

bool LS_optimizer::timeout() {
    return this->get_runtime() > this->timeout_s;
}

void LS_optimizer::random_reverse(std::vector<uint>::iterator it1, std::vector<uint>::iterator it2){
    std::uniform_int_distribution<uint> uni(0, 1);
    uint choice = uni(*this->rng);
    if (choice)
        std::reverse(it1, it2);
}

Solution LS_optimizer::make_solution(const vector<uint> &permutation) {
    Solution new_solution;
    new_solution.permutation = permutation;
    new_solution.is_feasible = this->instance->compute_fitness(permutation, &new_solution.fitness);
    return new_solution;
}

bool LS_optimizer::valid_solution(Solution *sol) {
    for (uint i = 0; i < this->instance->node_cnt; i++){
        if (sol->frequency[i] < this->instance->lbs[i] || sol->frequency[i] > this->instance->ubs[i])
            return false;
    }
    return true;
}

//**********************************************************************
// Logging
//**********************************************************************

void LS_optimizer::print_operation(bool update, const string& msg) {
    std::cout << (update ? "{*}" :  "{-}") << std::setw(6) << std::right << str(format("[%1%] ") % this->get_runtime());
    std::cout << std::setw(30) << std::left << msg;
    std::cout << " --> Feasible:" << this->solution.is_feasible <<
            "  Sol size: " << this->solution.permutation.size() <<
            "  BKS Fitness:" << this->best_known_solution.fitness <<
            "  Last update:" << std::chrono::duration_cast<std::chrono::seconds>(this->last_improvement - this->start).count() <<
            "  Curr Fitness:" << this->solution.fitness << std::endl;
}

void LS_optimizer::save_to_json(json& container) {
    std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(this->last_improvement-this->start);
    container["name"] = this->instance->name;
    container["last_improvement"] = sec.count();
    container["timeout"] = this->timeout_s;
    for (auto h : this->operation_histogram) {
        container["histogram"]["all"][h.first] = h.second.first;
        container["histogram"]["improving"][h.first] = h.second.second;
        container["histogram"]["rate"][h.first] = h.second.second > 0 ? (float)h.second.second/(float)h.second.first : 0;
    }
    for (auto s : this->steps) {
        container["steps"][itos(s.first)] = s.second;
    }
    this->best_known_solution.save_to_json(container["solution"]);
    container["config"] = this->config;
}


