//
// Created by honza on 11.03.22.
//

/* Copyright 2022, Gurobi Optimization, LLC */
/*
  Sudoku example.

  The Sudoku board is a 9x9 grid, which is further divided into a 3x3 grid
  of 3x3 grids.  Each cell in the grid must take a value from 0 to 9.
  No two grid cells in the same row, column, or 3x3 subgrid may take the
  same value.

  In the MIP formulation, binary variables x[i,j,v] indicate whether
  cell <i,j> takes value 'v'.  The constraints are as follows:
    1. Each cell must take exactly one value (sum_v x[i,j,v] = 1)
    2. Each value is used exactly once per row (sum_i x[i,j,v] = 1)
    3. Each value is used exactly once per column (sum_j x[i,j,v] = 1)
    4. Each value is used exactly once per 3x3 subgrid (sum_grid x[i,j,v] = 1)

  Input datasets for this example can be found in examples/data/sudoku*.
*/

#include <sstream>

#include "generic/solution.hpp"
#include "problem/sudoku.hpp"
#include "gurobi_c++.h"

using namespace std;

int ilp_minimize(SudokuInstance* inst, const string& model_filename = "", const string& result_filename = "", uint timeout=0)
{
    try {
        std::streambuf* buf;
        std::ofstream ofs;
        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);
        uint n = inst->node_cnt;
        uint sd = inst->subdivision;
        GRBVar vars[n][n][n];
        uint i, j, v;

        // Create 3-D array of model variables
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                for (v = 0; v < n; v++) {
                    string s = "G_" + uitos(i) + "_" + uitos(j) + "_" + uitos(v);
                    vars[i][j][v] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
                }
            }
        }

        // Add constraints
        // Each cell must take one value
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                GRBLinExpr expr = 0;
                for (v = 0; v < n; v++)
                    expr += vars[i][j][v];
                string s = "V_" + uitos(i) + "_" + uitos(j);
                model.addConstr(expr, GRB_EQUAL, 1.0, s);
            }
        }

        // Each value appears once per row
        for (i = 0; i < n; i++) {
            for (v = 0; v < n; v++) {
                GRBLinExpr expr = 0;
                for (j = 0; j < n; j++)
                    expr += vars[i][j][v];
                string s = "R_" + uitos(i) + "_" + uitos(v);
                model.addConstr(expr == 1.0, s);
            }
        }

        // Each value appears once per column
        for (j = 0; j < n; j++) {
            for (v = 0; v < n; v++) {
                GRBLinExpr expr = 0;
                for (i = 0; i < n; i++)
                    expr += vars[i][j][v];
                string s = "C_" + uitos(j) + "_" + uitos(v);
                model.addConstr(expr == 1.0, s);
            }
        }

        // Each value appears once per sub-grid
        for (v = 0; v < n; v++) {
            for (uint i0 = 0; i0 < sd; i0++) {
                for (uint j0 = 0; j0 < sd; j0++) {
                    GRBLinExpr expr = 0;
                    for (uint i1 = 0; i1 < sd; i1++) {
                        for (uint j1 = 0; j1 < sd; j1++) {
                            expr += vars[i0*sd+i1][j0*sd+j1][v];
                        }
                    }

                    string s = "Sub_" + uitos(v) + "_" + uitos(i0) + "_" + uitos(j0);
                    model.addConstr(expr == 1.0, s);
                }
            }
        }

        // Fix variables associated with pre-specified cells
        int val; // 0-based
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                val = inst->grid(i, j);
                val -= 1;
                if (val >= 0)
                    vars[i][j][val].set(GRB_DoubleAttr_LB, 1.0);
            }
        }

        // Optimize model
        model.optimize();
        // Report
        if (!result_filename.empty()) {
            ofs.open(result_filename);
            buf = ofs.rdbuf();
        } else {
            buf = std::cout.rdbuf();
        }
        std::ostream sout(buf);
        sout << "MIPGap: " << model.get(GRB_DoubleAttr_MIPGap) << std::endl;
        sout << "Runtime: " << model.get(GRB_DoubleAttr_Runtime)  << std::endl;
        sout << "Best solution: " << (int)round(model.get(GRB_DoubleAttr_ObjVal))  << std::endl;
        sout << "Best bound: " << (int)round(model.get(GRB_DoubleAttr_ObjBound))  << std::endl;
    } catch(GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }

    return 0;
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
    cout << "Usage: sudoku_ilp -d dataset_path [-t] timeout (sec) [-o] output file path\n";
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
    SudokuInstance inst = SudokuInstance(data_path, node_cnt);
    inst.print_nodes();
    if (timeout_s > 0) {
        ilp_minimize(&inst, filename, log_path, timeout_s);
    } else {
        ilp_minimize(&inst, filename, log_path);
    }
    return 0;
}