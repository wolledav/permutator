#include "optimizer.hpp"

#include <utility>

using permutator::fitness_t;
using boost::format;
using std::pair;
using std::vector;
using std::string;
using nlohmann::json;

//**********************************************************************************************************************
// INITIALIZATION
//**********************************************************************************************************************

Optimizer::Optimizer(Instance *inst, json config, uint seed) {
    this->instance = inst;
    this->config = std::move(config);
    Solution sol(inst->node_cnt);
    this->initial_solution = sol;
    this->current_solution = sol;
    this->best_known_solution = sol;
    this->rng = Optimizer::initRng(seed);
    this->setConstruction(this->config["construction"].get<string>());
    this->setMetaheuristic(this->config["metaheuristic"].get<string>());
    this->setLocalSearch(this->config["local_search"].get<string>());
    this->setPerturbations(this->config["perturbation"].get<vector<string>>());
    this->setOperators(this->config["operators"].get<vector<string>>());
    omp_set_num_threads(this->config["num_threads"]);
    if (this->config.contains("timeout")) {
        this->timeout_s = this->config["timeout"].get<uint>();
        std::cout << "Timeout set to " << this->timeout_s << "s" << std::endl;
    } else {
        std::cout << "No timeout set" << std::endl;
        this->timeout_s = UINT32_MAX;
    }
    this->unimproved_cnt = 0;
}

std::mt19937* Optimizer::initRng(uint seed) {
    if (seed == 0) {
        std::random_device rd;
        seed = rd();
    }
    return new std::mt19937(seed);
}

void Optimizer::setInitSolution(vector<uint> init_solution) {
    this->initial_solution = Solution(init_solution, *this->instance);
    this->initial_solution.print();
}


void Optimizer::setConstruction(const string& constr) {
    if (constr == "greedy")
        this->construction = [this] { constructGreedy(); };
    else if (constr == "random")
        this->construction = [this] { constructRandom(); };
    else if (constr == "randomReplicate")
        this->construction = [this] { constructRandomReplicate(); };
    else if (constr == "nearestNeighbor")
        this->construction = [this] { constructNearestNeighbor(); };
    else
        throw std::system_error(EINVAL, std::system_category(), constr);
}

void Optimizer::setMetaheuristic(const string& meta) {
    if (meta == "ILS")
        this->metaheuristic = [this] { ILS(); };
    else if (meta == "BVNS")
        this->metaheuristic = [this] { basicVNS(); };
    else
        throw std::system_error(EINVAL, std::system_category(), meta);
}

void Optimizer::setLocalSearch(const string& vnd_method) {
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

void Optimizer::setOperators(const vector<string>& operators) {
    this->operation_list = operators;
    if (this->operation_list.empty())
        throw std::system_error(EINVAL, std::system_category(), "ERROR: No neighborhood selected!");
    for (const auto& op_name : operators) {
        this->operation_histogram[op_name] = std::pair<uint, uint>(0, 0);
    }
}

void Optimizer::setPerturbations(const vector<string>& perturbations) {
    this->perturbation_list = perturbations;
    if (this->perturbation_list.empty())
        throw std::system_error(EINVAL, std::system_category(), "ERROR: No perturbation selected!");
}

void Optimizer::setLogger(std::basic_ostream<char> &logs) {
    if (this->log_stream == nullptr) {
        this->log_stream = &logs;
    } else {
        std::cout << "WARNING: Logger already set\n";
    }
}

void Optimizer::run() {
    this->start = std::chrono::steady_clock::now();
    this->last_improvement = this->start;
    this->construction();
    if (!this->stop()) this->metaheuristic();

    // Penalize solution under LB
    this->best_known_solution.fitness += this->instance->getLBPenalty(this->best_known_solution.frequency) * LB_PENALTY;
}

//**********************************************************************************************************************
// LOCAL SEARCH OPERATORS
//**********************************************************************************************************************

/*
 * Attempts to insert all nodes from A to X.
 * Performs the most improving (or least worsening) insertion.
 * If all nodes are at their upper bounds, does nothing.
 * Complexity: O(n^2)
 */
bool Optimizer::insert1() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%") % __func__));
#endif

    Solution best_solution = this->current_solution;
    vector<uint> perm = this->current_solution.permutation;
    vector<uint> freq = this->current_solution.frequency;
    fitness_t new_fitness;
    vector<fitness_t> new_penalties;
    bool updated = false;
    bool best_improved = false;

#pragma omp parallel for default(none) private(new_fitness, new_penalties) shared(best_solution, perm, freq, updated, best_improved)
    for (uint i = 0; i <= perm.size(); i++) {                       // for all positions i in X
        if (this->stop() || (best_improved && config["first_improve"])) continue;
        vector<uint> new_perm = perm;
        vector<uint> new_freq = freq;
        new_perm.insert(new_perm.begin() + i, 0);

        for (uint j = 0; j < this->instance->node_cnt; j++){    // for all nodes j in A
            if (this->current_solution.frequency[j] >= this->instance->ubs[j]) continue;
            new_perm[i] = j;
            new_freq[j]++;
            this->instance->computeFitness(new_perm, new_fitness, new_penalties);
            auto new_penalty = this->instance->getLBPenalty(new_freq);

#pragma omp critical
        {
            auto penalty = this->instance->getLBPenalty(best_solution.frequency);
            if ((new_penalty < penalty) || (new_penalty == penalty && new_fitness < best_solution.fitness)) {
                best_solution = Solution(new_perm, *this->instance);
                updated = true;
                if (new_fitness < best_solution.fitness) best_improved = true;
            }
        };
        new_freq[j]--;
        }
    }

    if (updated) this->current_solution = best_solution;

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif
    return updated;
}

/*
 * Attempts to append all nodes from A to X.
 * Performs the most improving (or least worsening) insertion.
 * If all nodes are at their upper bounds, does nothing.
 * Complexity: O(n)
 */
bool Optimizer::append1() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%") % __func__));
#endif

    Solution best_solution = this->current_solution;
    bool updated = false;
    bool best_improved = false;
    vector<uint> cand_perm = this->current_solution.permutation;
    vector<uint> cand_freq = this->current_solution.frequency;
    fitness_t cand_fitness;
    vector<fitness_t> cand_penalties;
    cand_perm.emplace_back(-1);

#pragma omp parallel for default(none) shared(best_solution, best_improved, updated) private(cand_fitness, cand_penalties) firstprivate(cand_perm, cand_freq)
    for (uint i = 0; i < this->instance->node_cnt; i++) {    // for all nodes i in A
        if (this->current_solution.frequency[i] >= this->instance->ubs[i] || (best_improved && config["first_improve"]) || this->stop()) continue;
        cand_perm.back() = i;
        cand_freq[i]++;
        this->instance->computeFitness(cand_perm, cand_fitness, cand_penalties);
        auto cand_penalty = this->instance->getLBPenalty(cand_freq);
#pragma omp critical
        {
            auto best_penalty = this->instance->getLBPenalty(best_solution.frequency);
            if ((cand_penalty < best_penalty) || (cand_penalty == best_penalty && cand_fitness < best_solution.fitness)) {
                best_solution = Solution(cand_perm, *this->instance);
                updated = true;
                if (cand_fitness < best_solution.fitness) best_improved = true;
            }
        };
        cand_freq[i]--;
    }

    if (updated) this->current_solution = best_solution;

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif

    return updated;
}

/*
 * Attempts to remove all nodes from X.
 * Performs the most-improving or non-worsening insertion.
 * Complexity: O(n)
 */
bool Optimizer::remove1() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%") % __func__));
#endif

    if (this->current_solution.permutation.empty()) return false;
    vector<uint> perm = this->current_solution.permutation;
    fitness_t fitness;
    vector<fitness_t> penalties;
    Solution best_solution = this->current_solution;
    bool updated = false;

#pragma omp parallel for default(none) private(fitness, penalties) shared(best_solution, perm, updated)
    for (uint i = 0; i < perm.size(); i++){
        if (this->stop() || (updated && config["first_improve"])) continue;
        if (this->current_solution.frequency[perm[i]] <= this->instance->lbs[perm[i]]) {
            continue;
        }
        vector<uint> new_perm = perm;
        new_perm.erase(new_perm.begin() + i);
        this->instance->computeFitness(new_perm, fitness, penalties);
#pragma omp critical
        if (fitness < best_solution.fitness) {
            best_solution = Solution(new_perm, *this->instance);
            updated = true;
        }
    }
    if (updated) this->current_solution = best_solution;


#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif
    return updated;
}

/*
 * Attempts to relocate all substring of length x in X.
 * Performs the most improving relocation.
 * If reverse = 1, reverts the substring before relocating.
 * Complexity: O(n^2)
 */
bool Optimizer::relocate(uint x, bool reverse) {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%%2%_%3%") % (reverse ? "r" : "") % __func__ % x));
#endif

    if (x > this->current_solution.permutation.size()) return false;

    fitness_t fitness;
    vector<fitness_t> penalties;
    vector<uint> perm = this->current_solution.permutation;
    Solution best_solution = this->current_solution;
    bool updated = false;

#pragma omp parallel for default(none) private(fitness, penalties) shared(best_solution, perm, x, reverse, updated)
    for (uint i = 0; i < perm.size() - x; i++) {
        if (this->stop() || (updated && config["first_improve"])) continue;
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
            this->instance->computeFitness(new_perm, fitness, penalties);
#pragma omp critical
            if (fitness < best_solution.fitness) {
                best_solution = Solution(new_perm, *this->instance);
                updated = true;
            }
        }
    }

    if (updated) this->current_solution = best_solution;

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif
    return updated;
}

/*
 * Attempts to revert the all possible substrings of length 2x + 1 around all nodes in X.
 * Performs the most improving reverse.
 * Complexity: O(n)
 */
bool Optimizer::centeredExchange(uint x) {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%_%2%") % __func__ % x));
#endif

    fitness_t fitness;
    vector<fitness_t> penalties;
    if (x > this->current_solution.permutation.size())
        return false;
    vector<uint> perm = this->current_solution.permutation;
    Solution best_solution = this->current_solution;
    bool updated = false;

#pragma omp parallel for default(none) private(fitness, penalties) shared(best_solution, perm, x, updated)
    for (int i = (int)x; i < (int)perm.size() - (int)x; i++) {
        if (this->stop() || (updated && config["first_improve"])) continue;
        vector<uint> new_perm = perm;
        for (int j = 1; j <= (int)x; j++) {
            new_perm[i+j] = perm[i-j];
            new_perm[i-j] = perm[i+j];
        }
        this->instance->computeFitness(new_perm, fitness, penalties);
#pragma omp critical
        if (fitness < best_solution.fitness) {
            best_solution = Solution(new_perm, *this->instance);
            updated = true;
        }
    }

    if (updated) this->current_solution=best_solution;

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif
    return updated;
}

/*
 * Attempts to exchange all substrings of lengths x and y.
 * Performs the most improving exchange.
 * If reverse = 1, reverts both substrings before exchange.
 * Complexity: O(n^2)
 */
bool Optimizer::exchange(uint x, uint y, bool reverse) {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%%2%_%3%_%4%") % (reverse ? "r" : "") % __func__ % x % y));
#endif

    if (x + y > this->current_solution.permutation.size())
        return false;
    fitness_t fitness;
    vector<fitness_t> penalties;
    vector<uint> perm = this->current_solution.permutation;
    Solution best_solution = this->current_solution;
    bool updated = false;

#pragma omp parallel for default(none) private(fitness, penalties) shared(best_solution, perm, x, y, reverse, updated)
    for (uint i = 0; i < perm.size() - x; i++) {
        if (this->stop() || (updated && config["first_improve"])) continue;
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
            this->instance->computeFitness(new_perm, fitness, penalties);
#pragma omp critical
            if (fitness < best_solution.fitness) {
                best_solution = Solution(new_perm, *this->instance);
                updated = true;
            }
        }
    }

    if (updated) this->current_solution=best_solution;

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif
    return updated;
}

/*
 * Attempts to move all occurrences of all nodes from A in X by [-x, x]\{0}.
 * Performs the most improving move.
 * Complexity: O(nx)
 */
bool Optimizer::moveAll(uint x) {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%_%2%") % __func__ % x));
#endif

    if (this->current_solution.permutation.size() < x)
        return false;
    vector<uint> perm = this->current_solution.permutation;
    fitness_t fitness;
    vector<fitness_t> penalties;
    Solution best_solution = this->current_solution;
    bool updated = false;

#pragma omp parallel for default(none) private(fitness, penalties) shared(best_solution, perm, x, updated)
    for (uint node_id = 0; node_id < this->instance->node_cnt; node_id++){ // try for all nodes
        if (this->stop() || (updated && config["first_improve"])) continue;
        // find all positions of node_id in perm
        vector<uint> positions;
        for (uint i = 0; i < perm.size(); i++){
            if (perm[i] == node_id){
                positions.push_back(i);
            }
        }
        // attempt all shifts by i from [-x, x]\{0}
        for (int i = -(int)x; i <= (int)x; i++){
            vector<uint> new_perm = perm;
            if (i == 0) continue; // no shift
            for (auto pos : positions){ // for all positions of node_id
                int new_pos = (int)pos + i; // shift by i
                if (new_pos < 0)
                    new_pos = (int)new_perm.size() + new_pos; // too far left -> overflow from right end
                else if ((uint)new_pos > new_perm.size() - 1)
                    new_pos = new_pos - (int)new_perm.size(); // too far right -> overflow from left end
                new_perm.erase(new_perm.begin() + pos);
                new_perm.insert(new_perm.begin() + new_pos, node_id);
            }
            // Evaluate
            this->instance->computeFitness(new_perm, fitness, penalties);
#pragma omp critical
            if (fitness < best_solution.fitness) {
                best_solution = Solution(new_perm, *this->instance);
                updated = true;
            }
        }
    }
    if (updated) this->current_solution=best_solution;

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif
    return updated;
}

/*
 * Attempts to exchange ids of all possible pairs of nodes X.
 * Performs the most improving exchange.
 * Complexity: O(n^2)
 */
bool Optimizer::exchangeIds() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%") % __func__));
#endif

    if (this->current_solution.permutation.size() < 2)
        return false;
    vector<uint> perm = this->current_solution.permutation;
    fitness_t fitness;
    vector<fitness_t> penalties;
    Solution best_solution = this->current_solution;
    bool updated = false;

#pragma omp parallel for default(none) private(fitness, penalties) shared(best_solution, perm, updated)
    for (uint i = 0; i < this->instance->node_cnt; i++) { // for all nodes i
        if (this->stop() || (updated && config["first_improve"])) continue;
        for (uint j = 0; j < i; j++) { // for all nodes j
            vector<uint> new_perm = perm;
            for (unsigned int & node : new_perm){
                if (node == i) {
                    node = j;
                } else if (node == j) {
                    node = i;
                }
            }
            this->instance->computeFitness(new_perm, fitness, penalties);
#pragma omp critical
            if (fitness < best_solution.fitness) {
                best_solution = Solution(new_perm, *this->instance);
                updated = true;
            }
        }
    }
    if (updated) this->current_solution = best_solution;

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif
    return updated;
}

/*
 * For all pairs of nodes i,j in A and for all numbers from {1,...,f_i} attempts to exchange first n occurrences of i and j.
 * Performs the most improving exchange.
 * Complexity: O(n^2)
 */
bool Optimizer::exchangeNIds() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%") % __func__));
#endif

    if (this->current_solution.permutation.size() < 2)
        return false;
    // Init structures
    vector<uint> perm = this->current_solution.permutation;
    fitness_t fitness;
    vector<fitness_t> penalties;
    Solution best_solution = this->current_solution;
    bool updated = false;

#pragma omp parallel for default(none) private(fitness, penalties) shared(best_solution, perm, updated)
    for (uint i = 0; i < this->instance->node_cnt; i++) { // for all nodes i in A
        if (this->stop() || (updated && config["first_improve"])) continue;
        for (uint j = 0; j < i; j++) { // for all pairs of nodes i,j in A
            for (uint n = 1; n < this->current_solution.frequency[i]; n++) { // for all frequencies up to f_i
                uint counter1 = 0, counter2 = 0;
                vector<uint> new_perm = perm;
                for (unsigned int &node: new_perm) { // for all nodes in new_perm
                    if (node == i && counter1 < n) { // if i swapped less than n times
                        counter1++;
                        node = j; // swap i for j
                    } else if (node == j && counter2 < n) { // if j swapped less than n times
                        counter2++;
                        node = i; // swap j for i
                    }
                }
                this->instance->computeFitness(new_perm, fitness, penalties);
#pragma omp critical
                if (fitness < best_solution.fitness) {
                    best_solution = Solution(new_perm, *this->instance);
                    updated = true;
                }
            }
        }
    }

    if (updated) this->current_solution=best_solution;

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif
    return updated;
}

/*
 * Attempts to revert all possible substrings of X.
 * Performs the most improving reverse.
 * Complexity: O(n^2)
 */
bool Optimizer::twoOpt() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\t\tO: %1%") % __func__));
#endif

    fitness_t fitness;
    vector<fitness_t> penalties;
    vector<uint> perm = this->current_solution.permutation;
    if (perm.size() < 2)
        return false;

    Solution best_solution = this->current_solution;
    bool updated = false;

#pragma omp parallel for default(none) private(fitness, penalties) shared(best_solution, perm, updated)
    for (uint i = 0; i <= perm.size() - 2; i++) {
        if (this->stop() || (updated && config["first_improve"])) continue;
            for (uint j = i+2; j <= perm.size(); j++) {
                vector<uint> new_perm = perm;
                reverse(new_perm.begin() + i, new_perm.begin() + j); // half closed interval [i, j)
                this->instance->computeFitness(new_perm, fitness, penalties);
#pragma omp critical
                if (fitness < best_solution.fitness) {
                    best_solution = Solution(new_perm, *this->instance);
                    updated = true;
                }
            }
    }

    if (updated) this->current_solution=best_solution;

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(updated);
#endif
    return updated;
}

// *********************************************************************************************************************
// PERTURBATIONS
// *********************************************************************************************************************

/*
 * Generates k random distinct indices.
 * Splits this->solution into k+1 substrings and reverts all of them, if reverse_all = 1,
 * otherwise reverts each one randomly with p = 0.5.
 */
void Optimizer::doubleBridge(uint k, bool reverse_all) {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tP: %1% %2%") % __func__ % k));
#endif

    if (k < 1) throw std::out_of_range("Double bridge: k < 1");
    std::uniform_int_distribution<uint> uni(0, this->current_solution.getSize() - 1);
    std::vector<uint> idx;
    vector<uint> new_perm = this->current_solution.permutation;

    // generate random indices
    for (uint i = 0; i < k; i++) {
        idx.push_back(uni(*rng));
    }
    std::sort(std::begin(idx), std::end(idx));
    // reverse subpermutations
    if (!reverse_all) {
        this->randomReverse(new_perm.begin(), new_perm.begin() + idx[0]); // half closed interval [i, j)
        for (uint i = 0; i + 1 < k; i++)
            this->randomReverse(new_perm.begin() + idx[i],
                                new_perm.begin() + idx[i + 1]); // half closed interval [i, j)
        this->randomReverse(new_perm.begin() + idx[k - 1], new_perm.end()); // half closed interval [i, j)
    } else {
        reverse(new_perm.begin(), new_perm.begin() + idx[0]); // half closed interval [i, j)
        for (uint i = 0; i + 1 < k; i++)
            reverse(new_perm.begin() + idx[i],
                                 new_perm.begin() + idx[i + 1]); // half closed interval [i, j)
        reverse(new_perm.begin() + idx[k - 1], new_perm.end()); // half closed interval [i, j)
    }
    // copy new solution
    this->current_solution = Solution(new_perm, *this->instance);

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(true);
#endif
}

/*
 * Randomly selects two nodes in X and swaps them.
 * Repeats k times.
 */
void Optimizer::randomSwap(uint k) {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tP: %1% %2%") % __func__ % k));
#endif

    uint sel1, sel2, temp;
    vector<uint> perm = this->current_solution.permutation;
    std::uniform_int_distribution<uint> uni_select(0, perm.size() - 1);

    for (uint i = 0; i < k; i++) {
        sel1 = uni_select(*this->rng);
        sel2 = uni_select(*this->rng);
        while (sel1 == sel2) { sel2 = uni_select(*this->rng); }
        temp = perm[sel1];
        perm[sel1] = perm[sel2];
        perm[sel2] = temp;
    }
    this->current_solution = Solution(perm, *this->instance);

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(true);
#endif
}

/*
 * Randomly selects a node in X and moves it to a random location.
 * Repeats k times.
 */
void Optimizer::randomMove(uint k) {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tP: %1% %2%") % __func__ % k));
#endif

    uint sel1, sel2, temp;
    vector<uint> perm = this->current_solution.permutation;
    std::uniform_int_distribution<uint> uni_select(0, perm.size() - 1);

    for (uint i = 0; i < k; i++) {
        sel1 = uni_select(*this->rng);
        sel2 = uni_select(*this->rng);
        while (sel1 == sel2) { sel2 = uni_select(*this->rng); }
        temp = perm[sel1];
        perm.erase(perm.begin() + sel1);
        perm.insert(perm.begin() + sel2, temp);
    }
    this->current_solution = Solution(perm, *this->instance);

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(true);
#endif
}

/*
 * Selects k distinct nodes from A.
 * Removes all their occurrences from X.
 * Reinserts them to randomly selected locations.
 */
void Optimizer::reinsert(uint k) {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tP: %1% %2%") % __func__ % k));
#endif

    vector<uint> idx_choice, node_cnt;
    vector<uint> perm = this->current_solution.permutation;

    // get nodes randomly
    std::uniform_int_distribution<uint> uni_select(0, this->instance->node_cnt-1);
    for (uint i = 0; i < k; i++) {
        idx_choice.push_back(uni_select(*this->rng));
        node_cnt.push_back(0);
    }
    // remove all selected nodes
    for (uint j = 0; j < k; j++) {
        for (uint i = 0; i < perm.size(); i++) {
            if (idx_choice[j] == perm[i]) {
                perm.erase(perm.begin() + i);
                node_cnt[j]++;
                i--; // prevents skipping neighboring occurrences
            }
        }
    }
    // insert selected nodes randomly
    uint node_id;
    for (uint j = 0; j < node_cnt.size(); j++) {
        for (uint i = 0; i < node_cnt[j]; i++) {
            node_id = idx_choice[j];
            std::uniform_int_distribution<uint> uni(0, std::max((int)perm.size() - 1, 0));
            perm.insert(perm.begin() + uni(*this->rng), node_id);
        }
    }
    this->current_solution = Solution(perm, *this->instance);

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(true);
#endif
}

/*
 * randomly selects a node node_id from A.
 * All occurrences of node_id are randomly moved in X up to a maximal distance k from their original locations.
 * This operation is performed k times.
 */
void Optimizer::randomMoveAll(uint k) {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tP: %1% %2%") % __func__ % k));
#endif

    int move;
    uint node_id;
    vector<uint> perm = this->current_solution.permutation;
    vector<uint> positions;
    std::uniform_int_distribution<uint> node_uni(0, this->instance->node_cnt - 1);
    std::uniform_int_distribution<int> dist_uni(-(int)k, (int)k);

    for (uint i = 0; i < k; i++) {
        // Select randomly a move distance and a node
        move = dist_uni(*this->rng);
        while (move == 0) move = dist_uni(*this->rng);
        node_id = node_uni(*this->rng);
        // Get all positions of node_id in this->solution
        positions.clear();
        for (uint j = 0; j < this->current_solution.permutation.size(); j++) {
            if (this->current_solution.permutation[j] == node_id) {
                positions.push_back(j);
            }
        }
        // Shift all positions by move
        for (auto &pos:positions) {
            int new_pos = (int) pos + move;
            new_pos = new_pos % (int)perm.size();
            if (new_pos < 0)
                new_pos = (int) perm.size() + new_pos;
            else if ((uint) new_pos > perm.size() - 1)
                new_pos = new_pos - (int) perm.size();
            pos = new_pos;
        }
        // Fill new_perm with node_id at new_pos
        std::vector<uint> new_perm(perm.size(), std::numeric_limits<uint>::max()/2);
        for (auto pos:positions) {
            new_perm[pos] = node_id;
        }
        // Fill the remaining nodes
        for (auto node:perm) {
            if (node != node_id) {
                for (auto &elem:new_perm) {
                    if (elem == std::numeric_limits<uint>::max()/2) {
                        elem = node;
                        break;
                    }
                }
            }
        }
        perm = new_perm;
    }
    this->current_solution = Solution(perm, *this->instance);

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(true);
#endif
}

//**********************************************************************************************************************
// LOCAL SEARCH HEURISTICS
//**********************************************************************************************************************

/*
 * Sequentially applies local search operators in fixed order.
 * Restarts from the first operator in case of improvement.
 * Terminates, when no operator improves the current solution.
 * Updates existing this->solution.
 */
void Optimizer::basicVND() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tLS: %1%") % __func__));
    std::cout << std::endl;
#endif

    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max()/2;
    fitness_t current_fitness = prev_fitness - 1;

    while (current_fitness < prev_fitness) {
        for (const auto &operation : this->operation_list) {
            if (this->stop()) return;
            if (this->operationCall(operation)) break;
        }
        prev_fitness = current_fitness;
        current_fitness = this->current_solution.fitness;
    }
}

/*
 * Sequentially applies local search operators in fixed order.
 * Reapplies the same operator in case of improvement.
 * Terminates, when no operator improves the current solution.
 * Updates existing this->solution.
 */
void Optimizer::pipeVND() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tLS: %1%") % __func__));
    std::cout << std::endl;
#endif

    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max()/2;
    fitness_t current_fitness = prev_fitness - 1;
    int last_improving_operator = -1;

    while (current_fitness < prev_fitness) {
        for (int i = 0; i < (int)this->operation_list.size(); i++) {
            if (last_improving_operator == i) return;
            string operation = this->operation_list[i];
            while (this->operationCall(operation)) {
                last_improving_operator = i;
                if (this->stop()) return;
            }
        }
        prev_fitness = current_fitness;
        current_fitness = this->current_solution.fitness;
    }
}

/*
 * Sequentially applies local search operators in fixed order.
 * Terminates, when no operator improves the current solution.
 * Updates existing this->solution.
 */
void Optimizer::cyclicVND() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tLS: %1%") % __func__));
    std::cout << std::endl;
#endif

    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max()/2;
    fitness_t current_fitness = prev_fitness - 1;

    while (current_fitness < prev_fitness) {
        for (const auto& operation : this->operation_list) {
            this->operationCall(operation);
            if (this->stop()) return;
        }
        prev_fitness = current_fitness;
        current_fitness = this->current_solution.fitness;
    }
}

/*
 * Sequentially applies local search operators in random order.
 * Terminates, when no operator improves the current solution.
 * Updates existing this->solution.
 */
void Optimizer::randomVND() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tLS: %1%") % __func__));
    std::cout << std::endl;
#endif

    std::uniform_int_distribution<uint> uni(0, this->operation_list.size() - 1);
    vector<uint> order; // order of local search operators
    for (uint i = 0; i < operation_list.size(); i++)
        order.push_back(i);
    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max()/2;
    fitness_t current_fitness = prev_fitness - 1;

    while (current_fitness < prev_fitness) {
        std::shuffle(order.begin(), order.end(), *this->rng); // shuffle order
        for (uint idx : order) {
            this->operationCall(this->operation_list[idx]);
            if (this->stop()) return;
        }
        prev_fitness = current_fitness;
        current_fitness = this->current_solution.fitness;
    }
}

/*
 * Sequentially applies local search operators in random order.
 * Repeats the same operator in case of improvement.
 * Terminates, when no operator improves the current solution.
 * Updates existing this->solution.
 */
void Optimizer::randompipeVND() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("\tLS: %1%") % __func__));
    std::cout << std::endl;
#endif

    std::uniform_int_distribution<uint> uni(0, this->operation_list.size() - 1);
    vector<uint> order;
    for (uint i = 0; i < operation_list.size(); i++)
        order.push_back(i);
    fitness_t prev_fitness = std::numeric_limits<fitness_t>::max()/2;
    fitness_t current_fitness = prev_fitness - 1;

    while (current_fitness < prev_fitness) {
        std::shuffle(order.begin(), order.end(), *this->rng);
        for (uint idx : order) {
            while (this->operationCall(this->operation_list[idx])) {
                if (this->stop()) { return; }
            }
        }
        prev_fitness = current_fitness;
        current_fitness = this->current_solution.fitness;
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
void Optimizer::ILS() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("MH: %1%") % __func__));
    std::cout << std::endl;
#endif
    fitness_t last_best_fitness;

    while (!this->stop()) {
        // Terminate, if stop_on_feasible set on true
        if (config["stop_on_feasible"] && this->best_known_solution.is_feasible) {
            std::cout << str(format("%1% found a feasible solution in %2% seconds ") % __func__ % this->getRuntime()) << std::endl;
            return;
        }

        // Perform local search on this->current_solution, ev. update this->best_known_solution
        last_best_fitness = best_known_solution.fitness;
        this->local_search();
        this->best_known_solution.fitness == last_best_fitness ? unimproved_cnt++ : unimproved_cnt = 0;


        // Reset this->current_solution to this->best_known_solution
        this->current_solution=this->best_known_solution;

        // Apply perturbation to this->current_solution
        for (const auto &pert : this->perturbation_list) {
            this->perturbationCall(pert, config["ils_k"].get<uint>());
        }
    }

    if (this->timeout_s == UINT32_MAX) {
        std::cout << __func__ << " terminated after " << UNIMPROVING_ITERS_MAX << " non-improving iterations" << std::endl;
    } else {
        std::cout << __func__ << " terminated after reaching " << this->timeout_s << "s timeout" << std::endl;
    }
}

/*
 * Basic Variable Neighborhood Search metaheuristic.
 * Alternates perturbation and local search.
 * Perturbation parameter k resets after improving local search, otherwise increases by 1.
 * Parameters:
 * bvns_min_k  . . . initial value of k
 * bvns_max_k . . . max value of k
 */
void Optimizer::basicVNS() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("MH: %1%") % __func__));
    std::cout << std::endl;
#endif

    uint min_k = config["bvns_min_k"].get<uint>();
    uint max_k = config["bvns_max_k"].get<uint>();
    uint k = min_k;
    fitness_t last_best_fitness;

    Solution current_best_solution = this->current_solution; // needed, as this->best_known_solution is updated internally
    while (!this->stop()) {
        // Terminate, if stop_on_feasible set on true
        if (config["stop_on_feasible"] && this->best_known_solution.is_feasible) {
            std::cout << str(format("%1% found a feasible solution in %2% seconds ") % __func__ % this->getRuntime()) << std::endl;
            return;
        }

        // Perform local search on this->current_solution, ev. update this->best_known_solution
        last_best_fitness = best_known_solution.fitness;
        this->local_search();
        this->best_known_solution.fitness == last_best_fitness ? unimproved_cnt++ : unimproved_cnt = 0;

        // Adjust k
        if (this->current_solution < current_best_solution){
            current_best_solution=this->current_solution;
            k = min_k;
        } else {
            k = k < max_k ? k + 1 : max_k;
        }

        // Reset this->solution to this->best_known_solution
        this->current_solution=this->best_known_solution;

        // Apply perturbation to this->current_solution
        for (const auto &pert : this->perturbation_list) {
            this->perturbationCall(pert, k);
        }
    }

    if (this->timeout_s == UINT32_MAX) {
        std::cout << __func__ << " terminated after " << UNIMPROVING_ITERS_MAX << " non-improving iterations" << std::endl;
    } else {
        std::cout << __func__ << " terminated after reaching " << this->timeout_s << "s timeout" << std::endl;
    }
}

//**********************************************************************************************************************
// CONSTRUCTIONS
//**********************************************************************************************************************

/*
 * Applies insert1 operator until the solution is not valid w.r.t. LBs and UBs or the operator does not update the solution.
 * Uses this->initial_solution and sets this->current solution.
 */
void Optimizer::constructGreedy() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("C: %1%") % __func__));
    std::cout << std::endl;
#endif
    this->current_solution = this->initial_solution;

    // Construct
    bool updated, valid;
    do {
        updated = insert1();
        valid = this->instance->FrequencyInBounds(this->current_solution.frequency);
    } while((!valid || updated) && !this->stop());

    // Evaluate
    if (this->current_solution < this->best_known_solution) {
        this->best_known_solution=this->current_solution;
        this->last_improvement = std::chrono::steady_clock::now();
        this->steps.emplace_back(this->getRuntime(), this->current_solution.fitness);
    }

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("C: %1%") % __func__));
    this->printResult(true);
#endif
}

/*
 * Applies append1 operator until the solution is not valid w.r.t. LBs and UBs or the operator does not update the solution.
 * Uses this->initial_solution and sets this->current solution.
 */
void Optimizer::constructNearestNeighbor() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("C: %1%") % __func__));
    std::cout << std::endl;
#endif
    this->current_solution = this->initial_solution;

    // Construct
    bool updated, valid;
    do {
        updated = append1();
        valid = this->instance->FrequencyInBounds(this->current_solution.frequency);
    } while((!valid || updated) && !this->stop());

    // Evaluate
    if (this->current_solution < this->best_known_solution) {
        this->best_known_solution=this->current_solution;
        this->last_improvement = std::chrono::steady_clock::now();
        this->steps.emplace_back(this->getRuntime(), this->current_solution.fitness);
    }

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("C: %1%") % __func__));
    this->printResult(true);
#endif
}

/*
 * Adds nodes to random positions of the solution, until all nodes are at their LBs.
 * Uses this->initial_solution and sets this->current solution.
 */
void Optimizer::constructRandom() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("C: %1%") % __func__));
#endif

    // Construct
    Solution sol = this->initial_solution;

    uint rnd_idx;
    for (uint node_id = 0; node_id < this->instance->node_cnt; node_id++) { // for all nodes
        while (sol.frequency[node_id] < this->instance->lbs[node_id]) {
            std::uniform_int_distribution<uint> uni(0, sol.getSize());
            rnd_idx = uni(*this->rng);
            sol.permutation.insert(sol.permutation.begin() + rnd_idx, node_id);
            sol.frequency[node_id]++;
        }
    }
    this->current_solution = Solution(sol.permutation, *this->instance);

    // Evaluate
    if (this->current_solution < this->best_known_solution) {
        this->best_known_solution=this->current_solution;
        this->last_improvement = std::chrono::steady_clock::now();
        this->steps.emplace_back(this->getRuntime(), this->current_solution.fitness);
    }

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(true);
#endif
}

/*
 * Creates a random permutation of nodes.
 * Replicates the permutation, until the solution is not valid w.r.t. LBs and UBs.
 * Uses this->initial_solution and sets this->current solution.
 */
void Optimizer::constructRandomReplicate() {
#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printOperation(str(format("C: %1%") % __func__));
#endif

    // Extract permutation from initial solution
    Solution sol(this->instance->node_cnt);
    for (auto node:this->initial_solution.permutation) {
        if (sol.frequency[node] == 0) {
            sol.permutation.push_back(node);
            sol.frequency[node]++;
        }
    }
    auto offset = sol.permutation.size();

    // Append missing nodes
    for (uint i = 0; i < this->instance->node_cnt; i++) {
        if (sol.frequency[i] == 0) {
            sol.permutation.push_back(i);
            sol.frequency[i]++;
        }
    }

    // Shuffle missing nodes
    auto begin = sol.permutation.begin();
    std::advance(begin, offset);
    std::shuffle(begin, sol.permutation.end(), *this->rng);

    // Replicate, until all nodes are within bounds
    auto perm = sol.permutation;
    sol = Solution(this->instance->node_cnt);
    do {
        for (auto elem : perm){
            if (sol.frequency[elem] < this->instance->lbs[elem]) {
                sol.frequency[elem]++;
                sol.permutation.push_back(elem);
            }
        }
    } while(!this->instance->FrequencyInBounds(sol.frequency) && !this->stop());

    // Evaluate
    this->current_solution = Solution(sol.permutation, *this->instance);
    if (this->current_solution < this->best_known_solution) {
        this->best_known_solution=this->current_solution;
        this->last_improvement = std::chrono::steady_clock::now();
        this->steps.emplace_back(this->getRuntime(), this->current_solution.fitness);
    }

#if defined STDOUT_ENABLED && STDOUT_ENABLED==1
    this->printResult(true);
#endif
}

//**********************************************************************
// UTILS
//**********************************************************************

bool Optimizer::operationCall(const string &operation_name) {
    bool result = false;

    // Terminate, if stop_on_feasible set on true
    if (config["stop_on_feasible"] && this->best_known_solution.is_feasible) {
        return result;
    }

    this->operation_histogram[operation_name].first++;
    result = this->operation_map.at(operation_name)();
    if (result)
        this->operation_histogram[operation_name].second++;
    if (this->current_solution < this->best_known_solution) {
        this->best_known_solution=this->current_solution;
        this->last_improvement = std::chrono::steady_clock::now();
        this->steps.emplace_back(this->getRuntime(), this->current_solution.fitness);
    }
    return result;
}

void Optimizer::perturbationCall(const string &perturbation_name, uint k) {
    if (!config["allow_infeasible"].get<bool>()) {
        do {
            this->current_solution=this->best_known_solution;
            this->perturbation_map.at(perturbation_name)(k); // call perturbation
        } while (!this->stop() && (this->current_solution.fitness == this->best_known_solution.fitness || !this->current_solution.is_feasible)); // no change or not feasible -> repeat
    } else {
        this->perturbation_map.at(perturbation_name)(k); // call perturbation
    }
}

long Optimizer::getRuntime() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - this->start).count();
}

bool Optimizer::stop() {
    if (timeout_s == UINT32_MAX) {
        return unimproved_cnt >= UNIMPROVING_ITERS_MAX;
    } else {
        return this->getRuntime() > this->timeout_s;
    }
}

void Optimizer::randomReverse(std::vector<uint>::iterator it1, std::vector<uint>::iterator it2) {
    std::uniform_int_distribution<uint> uni(0, 1);
    uint choice = uni(*this->rng);
    if (choice)
        std::reverse(it1, it2);
}

//**********************************************************************
// Logging
//**********************************************************************

void Optimizer::printOperation(const string &msg) {
    std::cout << str(format("[%1%s] ") % this->getRuntime()) << " ";
    std::cout << std::setw(30) << std::left << msg;
}

void Optimizer::printResult(bool update) {
    std::cout << str(format("-----> [%1%s]") % this->getRuntime()) <<
              "  Current: [" <<
              "fitness: " << this->current_solution.fitness <<
              "  updated: " << update <<
              "  feasible: " << this->current_solution.is_feasible <<
              "  size: " << this->current_solution.permutation.size() <<
              "]    BKS: [" <<
                "fitness: " << this->best_known_solution.fitness <<
                "  last updated: " << std::chrono::duration_cast<std::chrono::seconds>(this->last_improvement - this->start).count() << "s" <<
                "  feasible: " << this->best_known_solution.is_feasible <<
                "  size: " << this->best_known_solution.permutation.size() <<
                "]" << std::endl;
}

void Optimizer::saveToJson(json& container) {
    std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(this->last_improvement-this->start);
    container["name"] = this->instance->name;
    container["last_improvement"] = sec.count();
    container["timeout"] = this->timeout_s;
    for (const auto& h : this->operation_histogram) {
        container["histogram"]["all"][h.first] = h.second.first;
        container["histogram"]["improving"][h.first] = h.second.second;
        container["histogram"]["rate"][h.first] = h.second.second > 0 ? (float)h.second.second/(float)h.second.first : 0;
    }
    for (auto s : this->steps) {
        container["steps"][itos(s.first)] = s.second;
    }
    this->best_known_solution.saveToJson(container);
    container["config"] = this->config;
}

