#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

#include "generic/ASCHEA.hpp"
#include "generic/optimizer.hpp"
#include "src/problem/SUDOKU/SUDOKU.hpp"

using std::string;
using std::vector;
using nlohmann::json;

void parse_filename(string name, uint *N) {
    int start, i, len = 0;
    string temp;
    for (i = 0; name[i] < '0' || name[i] > '9'; i++) {}
    start = i;
    for (; name[i] >= '0' && name[i] <= '9'; i++, len++) {}
    temp = name.substr(start, len);
    *N = stoi(temp);
}


int main (int argc, char *argv[])
{
    uint node_cnt, seed = 0;
    uint timeout_s = UINT32_MAX;
    string data_path, output_path, conf_path;
    std::ofstream output_file, log_file;
    int opt;
    json config, output;
    string optimizer_type = "local";

    Instance::parseArgs(argc, argv, data_path, output_path, optimizer_type, config, seed); 
    
    string filename = getFilename(data_path);
    parse_filename(filename, &node_cnt);
    SudokuInstance inst = SudokuInstance(data_path, node_cnt);
    std::cout << "Solving " << inst.name << std::endl;
    
    BasicOptimizer* optimizer;
    if (optimizer_type == "local")
        optimizer = new Optimizer(&inst, config, seed);
    else if (optimizer_type == "evolutionary")
        optimizer = new ASCHEA(&inst, config, seed);
    
    optimizer->run();

    Solution sol = optimizer->getSolution();
    if (!output_path.empty()) {
        output_file.open(output_path);
        optimizer->saveToJson(output);
        output_file << output.dump(4);
    } else {
        inst.print_solution(&sol, std::cout);
        std::cout << sol.fitness << std::endl;
    }
    return 0;
}

