#include <iostream>
#include <boost/algorithm/string.hpp>
#include <fstream>

#include "generic/solution.hpp"
#include "src/problem/CVRP/CVRP.hpp"
#include "lib/tinyxml/tinyxml.h"
#include "gurobi_c++.h"

using namespace std;

int get_next(GRBVar **x, int N, int query) {
    for (int i = 0; i < N; i++) {
        if (x[query][i].get(GRB_DoubleAttr_X) == 1) {
            string name = x[query][i].get(GRB_StringAttr_VarName);
            std::vector<string> split_name;
            boost::split(split_name, name, boost::is_any_of("_"));
            return stoi(split_name[2]);
        }
    }
    return -1;
}

vector<uint> construct_permutation(CVRPInstance *inst, GRBVar **x, int N){
    vector<uint> perm;
    int next;
    perm.push_back(inst->depot_id);
    for (int i = 0; i < N; i++) {
        if (x[inst->depot_id][i].get(GRB_DoubleAttr_X) == 1) {
            perm.push_back(i);
            next = get_next(x, N, i);
            while(next > 0) {
                perm.push_back(next);
                next = get_next(x, N, next);
            }
            perm.push_back(inst->depot_id);
        }
    }
    return perm;
}

GRBModel* make_model(CVRPInstance* inst, GRBVar **x , const string& model_filename = "", bool save_model=false){
    uint N = inst->node_cnt;
    uint K = inst->tours;
    uint i, j;
    GRBEnv *env;
    auto *y = new GRBVar[N];
    env = new GRBEnv();
//    env->set("UserName", "wolledav");
    auto *model = new GRBModel(*env);
    GRBLinExpr obj = 0;
    // Create variables and objective function
    for (i = 0; i < N; i++) {
        if (i != inst->depot_id)
            y[i] = model->addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "y_"+itos(i));
        for (j = 0; j < N; j++) {
            x[i][j] = model->addVar(0, 1, 0, GRB_BINARY, "x_"+itos(i)+"_"+itos(j));
            obj += inst->dist_mat(i, j)*x[i][j];
        }
    }
    // Set objective function
    model->setObjective(obj, GRB_MINIMIZE);
    // (12) Exactly K cars leave depot
    GRBLinExpr expr = 0;
    for (j = 0; j < N; j++) {
        if (j == inst->depot_id) continue;
        expr += x[inst->depot_id][j];
    }
    model->addConstr(expr == K, uitos(K)+"_cars_leave_depot");
    // (13) Exactly K cars enter depot
    expr = 0;
    for (j = 0; j < N; j++) {
        if (j == inst->depot_id) continue;
        expr += x[j][inst->depot_id];
    }
    model->addConstr(expr == K, uitos(K)+"_cars_enter_depot");
    // (14) indegree constraint
    for (j = 0; j < N; j++) {
        expr = 0;
        if (j == inst->depot_id) continue;
        for (i = 0; i < N; i++)
            expr += x[i][j];
        model->addConstr(expr == 1, uitos(j)+"_is_entered_once");
    }
    // (15) outdegree constraint
    for (i = 0; i < N; i++) {
        expr = 0;
        if (i == inst->depot_id) continue;
        for (j = 0; j < N; j++)
            expr += x[i][j];
        model->addConstr(expr == 1, uitos(i)+"_is_left_once");
    }
    // (16) route continuity
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (i == inst->depot_id || j == inst->depot_id) continue;
            model->addConstr(y[j] - y[i] + inst->car_capacity * (1 - x[i][j]) >= inst->quantities[j]);
        }
    }
    if (save_model) {
        if (!model_filename.empty())
            model->write("../model/" + model_filename + ".mps");
        model->write("../model/recent.mps");
    }
    return model;
}

void ilp_minimize(CVRPInstance* inst, const string& model_filename = "", const string& result_filename = "", uint timeout=0){
    uint N = inst->node_cnt;
    std::streambuf* buf;
    std::ofstream ofs;
    try {
        GRBVar** x = new GRBVar*[N];
        for (uint i = 0; i < N; i++) {
            x[i] = new GRBVar[N];
        }
        GRBModel* model = make_model(inst, x, model_filename, true);
        model->set(GRB_IntParam_Threads, GUROBI_THREAD_CNT);
        if (timeout > 0)
            model->getEnv().set(GRB_DoubleParam_TimeLimit, timeout);
        // Optimize model
        model->optimize();
        // Report

        if (!result_filename.empty()) {
            ofs.open(result_filename);
            buf = ofs.rdbuf();
        } else {
            buf = std::cout.rdbuf();
        }
        std::ostream sout(buf);
        sout << "MIPGap: " << model->get(GRB_DoubleAttr_MIPGap) << std::endl;
        sout << "Runtime: " << model->get(GRB_DoubleAttr_Runtime)  << std::endl;
        sout << "Best solution: " << (int)round(model->get(GRB_DoubleAttr_ObjVal))  << std::endl;
        sout << "Best bound: " << (int)round(model->get(GRB_DoubleAttr_ObjBound))  << std::endl;
        // Print solution
        vector<uint> perm = construct_permutation(inst, x, N);
        Solution sol((vector<uint> &) perm, *inst);
        inst->print_solution(&sol, sout);
    } catch (GRBException e) {
        cout << "Error number: " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
        exit(EXIT_FAILURE);
    } catch (...) {
        cout << "Unknown error during optimization." << endl;
        exit(EXIT_FAILURE);
    }
}

void parse_filename(string name, uint *N, uint *k) {
    int start, i, len = 0;
    string temp;
    for (i = 0; name[i] != 'n'; i++) {}
    start = i+1;
    for (i++; name[i] >= '0' && name[i] <= '9'; i++, len++) {}
    temp = name.substr(start, len);
    *N = stoi(temp);
    for (i++; name[i] != 'k'; i++) {}
    start = i+1; len = 0;
    for (i++; name[i] >= '0' && name[i] <= '9'; i++, len++) {}
    temp = name.substr(start, len);
    *k = stoi(temp);
}

void show_usage(){
    cout << "Usage: CVRP_ilp -d dataset_path [-t] timeout (sec) [-o] output file path\n";
}

int main(int argc, char *argv[])
{
    string data_path, log_path;
    uint node_cnt, tours, timeout_s=0;
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
    parse_filename(filename, &node_cnt, &tours);
    CVRPInstance inst = CVRPInstance(data_path.c_str(), node_cnt, tours);
    inst.print_nodes();
    if (timeout_s > 0) {
        ilp_minimize(&inst, filename, log_path, timeout_s);
    } else {
        ilp_minimize(&inst, filename, log_path);
    }
    return 0;
}