#include "NPFS.hpp"

using std::vector;
using std::string;
using permutator::fitness_t;
using boost::format;

NPFSInstance::NPFSInstance(const string& path, uint jobs, uint machines)
        : Instance(generateName(path, "NPFS"), jobs, machines, machines){
    this->big_m = 0;
    this->job_mat.resize(machines, jobs);
    this->read(path.c_str());
    if (machines != this->machine_cnt || jobs != this->job_cnt) {
        std::cerr << str(format("ERROR: File name does not correspond with it's contents: %s\n") % path);
        throw std::exception();
    }
}

void NPFSInstance::read(const char *path) {
    int dummy;
    std::ifstream ifs(path, std::ifstream::in);
    if (!ifs.good())
        throw std::system_error(ENOENT, std::system_category(), path);
    ifs >> this->job_cnt;
    ifs >> this->machine_cnt;
    for (uint j = 0; j < this->job_cnt; j++) {
        for (uint m = 0; m < this->machine_cnt; m++) {
            ifs >> dummy;
            ifs >> this->job_mat(m, j);
            this->big_m += this->job_mat(m, j);
        }
    }
    this->job_duplication_penalty = 2*this->big_m;
    this->empty_machine_penalty = 3*this->job_cnt*this->big_m;
}

bool NPFSInstance::computeFitness(const vector<uint> &permutation, fitness_t *fitness) {
    uint perm_idx, end, curr_job, prev_job, last_machine = 0;
    vector<bool> processed_jobs (this->job_cnt, false);
    *fitness = 0;
    boost::numeric::ublas::matrix<uint> op_end_times (this->machine_cnt, this->job_cnt, 0);
    // First machine has no waiting time between operations.
    end = std::min((uint)permutation.size(), this->job_cnt);
    op_end_times(0, permutation[0]) = this->job_mat(0, permutation[0]);
    processed_jobs[permutation[0]] = true;
    for (uint j = 1; j < end; j++) {
        if (processed_jobs[permutation[j]])
            *fitness += this->job_duplication_penalty;
        else
            processed_jobs[permutation[j]] = true;
        op_end_times(0, permutation[j]) = op_end_times(0, permutation[j - 1]) + this->job_mat(0, permutation[j]);
    }
    if (end < this->job_cnt)
        *fitness += (this->job_cnt - end)*this->big_m;
    // Compute op_end_time for the rest of the machines
    for (uint m = 1; m < this->machine_cnt; m++) {
        last_machine = m;
        end = std::min((uint)permutation.size(), (m+1)*this->job_cnt);
        std::fill(processed_jobs.begin(), processed_jobs.end(), false);
        if (permutation.size() > m*this->job_cnt) {
            for (perm_idx = m * this->job_cnt; perm_idx < end; perm_idx++) {
                curr_job = permutation[perm_idx], prev_job = permutation[perm_idx - 1];
                // Every machine can process each job only once.
                if (processed_jobs[curr_job])
                    *fitness += this->job_duplication_penalty;
                else
                    processed_jobs[curr_job] = true;
                // If the job has not yet ended on previous machine wait for it to finish.
                if (op_end_times(m-1, curr_job) > op_end_times(m, prev_job))
                    op_end_times(m, curr_job) = op_end_times(m - 1, curr_job) + this->job_mat(m, curr_job);
                // Else start next job right away.
                else
                    op_end_times(m, curr_job) = op_end_times(m, prev_job) + this->job_mat(m, curr_job);
            }
            if (perm_idx != (m+1)*this->job_cnt)
                *fitness += ((m+1)*this->job_cnt - end)*this->big_m;
        } else {
            *fitness += this->empty_machine_penalty;
        }
    }
    if (last_machine > 0) *fitness += std::max(op_end_times(last_machine, permutation[(end - 1)]),
                                                op_end_times(last_machine-1, permutation[(end - 1)]));
    else *fitness += op_end_times(last_machine, permutation[(end - 1)]);
    return *fitness < this->big_m;
}

void NPFSInstance::print_solution(Solution *solution, std::basic_ostream<char> &outf) const {
    if (solution->permutation.size() == this->machine_cnt*this->job_cnt) {
        outf << "Fitness: " << solution->fitness << "\nPermutation:\n";
        for (uint i = 0; i < this->machine_cnt; i++) {
            outf << "M-" << i << ": ";
            for (uint j = 0; j < this->job_cnt; j++) {
                outf << solution->permutation[i * job_cnt + j] << " ";
            }
            outf << std::endl;
        }
        outf << std::endl;
    } else {
        for (auto elem : solution->permutation) {
            outf << elem << " ";
        }
        outf << std::endl;
    }
}

void NPFSInstance::print_nodes() {
    std::cout << "Job matrix\n";
    std::cout << "\t";
    for (uint i = 1; i <= this->node_cnt; i++)
        std::cout << std::setw(4)  << i;
    std::cout << std::endl;
    for (uint i = 0; i < this->machine_cnt; i++){
        std::cout << i+1 << "\t";
        for (uint j = 0; j < this->job_cnt; j++) {
            std::cout << std::setw(4) << this->job_mat(i, j);
        }
        std::cout << std::endl;
    }
}
