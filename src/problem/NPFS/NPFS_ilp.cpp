#include <iostream>
#include <boost/algorithm/string.hpp>
#include <numeric>      // std::iota

#include "generic/solution.hpp"
#include "src/problem/NPFS/NPFS.hpp"
#include "gurobi_c++.h"

using namespace std;


template <typename T>
vector<uint> sort_indexes(const vector<T> &v) {

    // initialize original index locations
    vector<uint> idx(v.size());
    iota(idx.begin(), idx.end(), 0);

    // sort indexes based on comparing values in v
    // using std::stable_sort instead of std::sort
    // to avoid unnecessary index re-orderings
    // when v contains elements of equal values
    stable_sort(idx.begin(), idx.end(),
                [&v](uint i1, uint i2) {return v[i1] < v[i2];});

    return idx;
}

vector<uint> construct_permutation(GRBVar *x, uint job_cnt, uint machine_cnt){
    vector<uint> perm(job_cnt*machine_cnt);
    for (uint i = 0; i < machine_cnt; i++) {
        vector<double> job_times(job_cnt);
        for (uint j = 0; j < job_cnt; j++)
            job_times[j] = x[i*job_cnt + j].get(GRB_DoubleAttr_X);
        vector<uint> idx = sort_indexes(job_times);
        copy_n(idx.begin(), job_cnt, perm.begin() + i*job_cnt);
    }
    return perm;
}

vector<double> report_end_times(NPFSInstance* inst, GRBVar *x, const vector<uint>& permutation) {
    vector<double> end_times (permutation.size());
    cout << "Operation end times:" << endl;
    for (uint i = 0; i < inst->machine_cnt; i++) {
        cout << "M-" << i << ": ";
        for (uint j = 0; j < inst->job_cnt; j++) {
            uint idx = permutation[i*inst->job_cnt + j];
            end_times[i*inst->job_cnt + j] = x[i*inst->job_cnt + idx].get(GRB_DoubleAttr_X) + inst->job_mat(i, idx);
            cout << end_times[i*inst->job_cnt + j] << " ";
        }
        cout << std::endl;
    }
    return end_times;
}

GRBModel* make_model(NPFSInstance* inst, GRBVar *x , const string& model_filename = "", bool save_model=false){
    uint M = inst->machine_cnt;
    uint N = inst->job_cnt;
    uint i, j, k;
    GRBVar *y = new GRBVar[M*N*N];
    // Set objective function
    GRBEnv *env;
    env = new GRBEnv();
    auto *model = new GRBModel(*env);
    GRBVar C_max = model->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "C_max");
    GRBLinExpr obj = C_max;
    model->setObjective(obj, GRB_MINIMIZE);
    //model->read("../model/QAPtune1.prm");
    GRBLinExpr expr;
    // Create variables
    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            x[i*N + j] = model->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS,
                                       str(format("x_%1%_%2%") % i % j));
            for (k = 0; k < N; k++)
                y[i*N*N + j*N + k] = model->addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_BINARY,
                                                   str(format("y_%1%_%2%_%3%") % i % j % k));
        }
    }
    //  (2) define the makespan
    for (j = 0; j < N; j++) {
        expr = x[(M-1)*N + j] + inst->job_mat(M-1, j);
        model->addConstr(expr <= C_max, str(format("x_%1%_%2%+p_%1%_%2%<=C_max") % i % j));
    }
    //  (3) require a job to terminate on a machine before starting on the following one
    for (i = 0; i < M - 1; i++) {
        for (j = 0; j < N; j++) {
            expr = x[i*N + j] + inst->job_mat(i, j);
            model->addConstr(expr <= x[(i+1)*N + j], str(format("x_%1%_%2%+p_%1%_%2%<=x_%3%_%2%") % i % j % (i+1)));
        }
    }
    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            expr = x[i * N + j] + inst->job_mat(i, j);
            for (k = 0; k < N; k++) {
                if (k != j) // (4) guarantees that the jobs are processed in the order defined by the variables y,
                    model->addConstr(expr <= x[i * N + k] + inst->big_m*(1 - y[i*N*N + j*N + k]),
                        str(format("x_%1%_%2%+p_%1%_%2%<=x_%1%_%3%+M*(1-y_%1%_%2%_%3%)") % i % j % k));
                if (k < j) //  (5) forces a linear ordering of the jobs on all machines.
                        model->addConstr( y[i*N*N + j*N + k] + y[i*N*N + k*N + j] == 1,
                         str(format("y_%1%_%2%_%3%)+y_%1%_%3%_%2%)==1") % i % j % k));
            }
        }
    }
    if (save_model) {
        if (!model_filename.empty())
            model->write("../model/" + model_filename + ".mps");
        model->write("../model/recent.lp");
    }
    return model;
}

void ilp_minimize(NPFSInstance* inst, const string& model_filename = "", const string& result_filename = "", uint timeout=0){
    std::streambuf* buf;
    std::ofstream ofs;
    GRBVar* x = new GRBVar[inst->machine_cnt*inst->job_cnt];
    // Report stream
    if (!result_filename.empty()) {
        ofs.open(result_filename);
        buf = ofs.rdbuf();
    } else {
        buf = std::cout.rdbuf();
    }
    std::ostream sout(buf);
    try {
        GRBModel* model = make_model(inst, x, model_filename, true);
        model->set(GRB_IntParam_Threads, GUROBI_THREAD_CNT);
        if (timeout > 0)
            model->getEnv().set(GRB_DoubleParam_TimeLimit, timeout);
        // Optimize model
        model->optimize();
        if (model->get(GRB_IntAttr_Status) == GRB_LOADED) {
            sout << "Model loaded but optimization didn't start." << std::endl;
        } else {
            sout << "MIPGap: " << model->get(GRB_DoubleAttr_MIPGap) << std::endl;
            sout << "Runtime: " << model->get(GRB_DoubleAttr_Runtime) << std::endl;
            sout << "Best solution: " << (int) round(model->get(GRB_DoubleAttr_ObjVal)) << std::endl;
            sout << "Best bound: " << (int) round(model->get(GRB_DoubleAttr_ObjBound)) << std::endl;
        }
        // Print solution
//         vector<uint> perm = construct_permutation(x, inst->job_cnt, inst->machine_cnt);
//         report_end_times(inst, x, perm);
//         fitness_t fit;
//         bool is_feasible = inst->compute_fitness(perm, &fit);
//         Solution sol(inst->node_cnt, perm, fit, is_feasible);
//         inst->print_solution(&sol, sout);
    } catch (GRBException e) {
        cout << "Error number: " << e.getErrorCode() << endl;
        cout << "Message: " << e.getMessage() << endl;
        sout << "Error number: " << e.getErrorCode() << endl;
        sout << "Message: " << e.getMessage() << endl;
    } catch (...) {
        cout << "Unknown error during optimization." << endl;
    }
}

void parse_filename(string name, uint *N, uint *M) {
    int start, i, len = 0;
    string temp;
    for (i = 0; name[i] < '0' || name[i] > '9'; i++) {}
    start = i;
    for (; name[i] >= '0' && name[i] <= '9'; i++, len++) {}
    temp = name.substr(start, len);
    *N = stoi(temp);
    for (; name[i] < '0' || name[i] > '9'; i++) {}
    start = i; len = 0;
    for (; name[i] >= '0' && name[i] <= '9'; i++, len++) {}
    temp = name.substr(start, len);
    *M = stoi(temp);
}

void show_usage(){
    cout << "Usage: NPFS_ilp -d dataset_path [-t] timeout (sec) [-o] output file path\n";
}

int main(int argc, char *argv[])
{
    string data_path, log_path;
    uint job_cnt, machine_cnt, timeout_s=0;
    int opt;
    // Parse arguments
    while ((opt = getopt(argc, argv, "d:t:o:")) != -1) {
        switch (opt) {
            case 'd':
                data_path = optarg;
                break;
            case 't':
                timeout_s = strtoul(optarg, nullptr, 10);
                break;
            case 'o':
                log_path = optarg;
                break;
            default: /* '?' */
                std::cout << "Unknown arg option: " << (char)opt << std::endl;
                show_usage();
                exit(EXIT_FAILURE);
        }
    }
    if (data_path.empty()) {
        show_usage();
        exit(EXIT_FAILURE);
    }
    string filename = get_filename(data_path);
    std::cout << filename << std::endl;
    parse_filename(filename, &job_cnt, &machine_cnt);
    NPFSInstance inst = NPFSInstance(data_path.c_str(), job_cnt, machine_cnt);
    if (timeout_s > 0) {
        ilp_minimize(&inst, filename, log_path, timeout_s);
    } else {
        ilp_minimize(&inst, filename, log_path);
    }
    return 0;
}