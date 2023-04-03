#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

#include "generic/ASCHEA.hpp"
#include "generic/optimizer.hpp"
#include "src/problem/EVRP/EVRP.hpp"

using std::string;
using std::vector;
using nlohmann::json;

void parse_filename(string name, uint *N, uint *S) {
    int start, i, len = 0;
    string temp;
    for (i = 0; name[i] != 'n'; i++) {}
    start = i+1;
    for (i++; name[i] >= '0' && name[i] <= '9'; i++, len++) {}
    temp = name.substr(start, len);
    *N = stoi(temp);
    for (i++; name[i] != 's'; i++) {}
    start = i+1; len = 0;
    for (i++; name[i] >= '0' && name[i] <= '9'; i++, len++) {}
    temp = name.substr(start, len);
    *S = stoi(temp);
}


int main (int argc, char *argv[])
{
    uint customer_cnt, charger_cnt, seed = 0;
    uint timeout_s = UINT32_MAX;
    string data_path, output_path, conf_path;
    std::ofstream output_file, log_file;
    json config, output;
    string optimizer_type = "local";

    Instance::parseArgs(argc, argv, data_path, output_path, optimizer_type, config, seed);    

    // Parse and solve instance
    string filename = getFilename(data_path);
    parse_filename(filename, &customer_cnt, &charger_cnt);
    EVRPInstance inst = EVRPInstance(data_path.c_str(), customer_cnt, charger_cnt);
    if (STDOUT_ENABLED) std::cout << "Solving " << inst.name << std::endl;
    
    BasicOptimizer* optimizer;
    if (optimizer_type == "local")
        optimizer = new Optimizer(&inst, config, seed);
    else if (optimizer_type == "evolutionary")
        optimizer = new ASCHEA(&inst, config, seed);
    
    optimizer->run();

    // Export solution
    Solution sol = optimizer->getSolution();
    if (!output_path.empty()) {
        output_file.open(output_path);
        optimizer->saveToJson(output);
        output_file << output.dump(4);
    } else {
        if (STDOUT_ENABLED) inst.print_solution(&sol, std::cout);
        std::cout << sol.fitness << std::endl;
    }
    return 0;
} 