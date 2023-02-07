#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include "generic/solution.hpp"
#include "src/problem/QAP/QAP.hpp"
#include "gurobi_c++.h"

using namespace std;

typedef boost::numeric::ublas::matrix<uint> uimat;

uint max_inner_prod(const std::vector<uint> &a, const std::vector<uint> &b){
    uint res = 0;
    for (uint i = 0; i < a.size(); i++) {
        res += a[i] * b[i];
    }
    return res;
}

uint min_inner_prod(const std::vector<uint> &a, const std::vector<uint> &b){
    uint res = 0;
    for (uint i = 0; i < a.size(); i++) {
        res += a[i] * b[b.size()-(i+1)];
    }
    return res;
}

void print_matrix(const uimat &m, const string& name)
{
    std::cout << name << std::endl;
    std::cout << "\t";
    for (uint i = 1; i <= m.size1(); i++)
        std::cout << std::setw(8)  << i;
    std::cout << std::endl;
    for (uint i = 0; i < m.size1(); i++){
        std::cout << i+1 << "\t";
        for (uint j = 0; j < m.size2(); j++) {
            std::cout << std::setw(8) << m(i, j);
        }
        std::cout << std::endl;
    }
}

void get_mat_row(uimat &source, std::vector<uint> &target, uint row_id) {
    std::copy((source.begin1()+row_id).begin(), (source.begin1()+row_id).end(), target.begin());
    target.erase(target.begin() + row_id);
}

void compute_bounds(uimat &LB, uimat &UB, uimat &flow_mat, uimat &dist_mat){
    uint N = flow_mat.size1();
    for (uint i = 0; i < N; i++) {
        for (uint j = 0; j < N; j++) {
            std::vector<uint> a(flow_mat.size1());
            std::vector<uint> b(dist_mat.size1());
            get_mat_row(flow_mat, a, i);
            get_mat_row(dist_mat, b, j);
            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());
            LB(i, j) = min_inner_prod(a, b);
            UB(i, j) = max_inner_prod(a, b);
        }
    }
}

vector<uint> construct_permutation(GRBVar *x, uint N){
    vector<uint> perm;
    for (uint i = 0; i < N; i++) {
        for (uint j = 0; j < N; j++) {
            if (x[i*N + j].get(GRB_DoubleAttr_X) == 1) {
                perm.push_back(j);
                break;
            }
        }
    }
    return perm;
}

GRBModel* make_model(QAPInstance* inst, GRBVar *x , const string& model_filename = "", bool save_model=false){
    uint N = inst->node_cnt;
    uint i, j;
    boost::numeric::ublas::matrix<uint> LB(N, N), UB(N, N);
    compute_bounds(LB, UB, inst->flow_mat, inst->dist_mat);
    GRBVar *y = new GRBVar[N*N];
    std::vector<uint> a(inst->flow_mat.size1());
    std::vector<uint> b(inst->dist_mat.size1());
    GRBEnv *env;

    env = new GRBEnv();
    auto *model = new GRBModel(*env);
    //model->read("../model/QAPtune1.prm");
    GRBLinExpr expr, obj = 0;
    // Create variables and objective function
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            x[i*N + j] = model->addVar(0, 1, 0, GRB_BINARY, "x_" + itos(i) + "_" + itos(j));
            y[i*N + j] = model->addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "y_" + itos(i) + "_" + itos(j));
            obj += y[i*N + j] + inst->flow_mat(i, i)*inst->dist_mat(j, j)*x[i*N + j] ;
        }
    }
    // Set objective function
    model->setObjective(obj, GRB_MINIMIZE);
    //  y UB and LB
    for (i = 0; i < N ; i++) {
        for (j = 0; j < N; j++) {
            expr = 0;
            for (uint k = 0; k < N; k++) {
                if (k == i) continue;
                for (uint l = 0; l < N; l++) {
                    if (l == j) continue;
                    expr += inst->flow_mat(i, k) * inst->dist_mat(j, l) * x[k * N + l];
                }
            }
            expr -= UB(i, j) * (1 - x[i * N + j]) ;
            model->addConstr(y[i*N+j] >= expr, "UB(y_"+itos(i)+"_"+ itos(j) + ")");
            model->addConstr( y[i*N+j] >= LB(i, j)*x[i*N + j] , "LB(y_"+itos(i)+"_"+ itos(j) + ")");
        }
    }
    // x is a permutation matrix
    for (i = 0; i < N; i++) {
        expr = 0;
        for (j = 0; j < N; j++) {
            expr += x[i*N + j];
        }
        model->addConstr(expr == 1, "x_" + itos(i) + "th row == 1");
    }
    for (j = 0; j < N; j++) {
        expr = 0;
        for (i = 0; i < N; i++) {
            expr += x[i*N + j];
        }
        model->addConstr(expr == 1, "x_"+itos(j)+"th col == 1");
    }
    if (save_model) {
        if (!model_filename.empty())
            model->write("../model/" + model_filename + ".mps");
        model->write("../model/recent.lp");
    }
    return model;
}

void ilp_minimize(QAPInstance* inst, const string& model_filename = "", const string& result_filename = "", uint timeout=0){
    uint N = inst->node_cnt;
    std::streambuf* buf;
    std::ofstream ofs;
    GRBVar* x = new GRBVar[N*N];
    try {
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
        vector<uint> perm = construct_permutation(x, N);
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

void parse_filename(string name, uint *N) {
    int start, i, len = 0;
    string temp;
    for (i = 0; name[i] < '0' || name[i] > '9'; i++) {}
    start = i;
    for (; name[i] >= '0' && name[i] <= '9'; i++, len++) {}
    temp = name.substr(start, len);
    *N = stoi(temp);
}

void show_usage(){
    cout << "Usage: QAP_ilp -d dataset_path [-t] timeout (sec) [-o] output file path\n";
}

int main(int argc, char *argv[])
{
    string data_path, log_path;
    uint node_cnt, timeout_s=0;
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
    parse_filename(filename, &node_cnt);
    QAPInstance inst = QAPInstance(data_path.c_str(), node_cnt);
    if (timeout_s > 0) {
        ilp_minimize(&inst, filename, log_path, timeout_s);
    } else {
        ilp_minimize(&inst, filename, log_path);
    }
    return 0;
}