#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

#include "generic/EA.hpp"
#include "generic/optimizer.hpp"
#include "src/problem/NPFS/NPFS.hpp"

using std::string;
using std::vector;
using nlohmann::json;

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


int main (int argc, char *argv[])
{
    uint job_cnt, machine_cnt, seed = 0;
    uint timeout_s = UINT32_MAX;
    string data_path, output_path, conf_path;
    std::ofstream output_file, log_file;
    json config, output;
    string optimizer_type = "local";

    Instance::parseArgs(argc, argv, data_path, output_path, optimizer_type, config, seed);  

    string filename = getFilename(data_path);
    parse_filename(filename, &job_cnt, &machine_cnt);
    NPFSInstance inst = NPFSInstance(data_path, job_cnt, machine_cnt);
    std::cout << "Solving " << inst.name << std::endl;
    
     BasicOptimizer* optimizer;
    if (optimizer_type == "local")
        optimizer = new Optimizer(&inst, config, seed);
    else if (optimizer_type == "evolutionary")
        optimizer = new EA(&inst, config, seed);
    
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