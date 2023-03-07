
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>


#include "generic/EA.hpp"
#include "generic/optimizer.hpp"
#include "src/problem/ROADEF/ROADEF.hpp"


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
    //string filename = getFilename(data_path);
    ROADEFInstance inst = ROADEFInstance(data_path.c_str());
    std::cout << "Solving " << inst.name << std::endl;
    
    BasicOptimizer* optimizer;
    if (optimizer_type == "local")
        optimizer = new Optimizer(&inst, config, seed);
    else if (optimizer_type == "evolutionary")
        optimizer = new EA(&inst, config, seed);
    
    optimizer->run();

    // Export solution
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
