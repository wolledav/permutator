#include <iostream>
#include <string>

#include "generic/ASCHEA.hpp"
#include "generic/optimizer.hpp"
#include "src/problem/wTSPSD/wTSPSD.hpp"

using std::string;
using nlohmann::json;


int main (int argc, char *argv[]) {
    
    uint seed = 0;
    string data_path, output_path, conf_path;
    std::ofstream output_file;
    json config, output;
    string optimizer_type = "local";
    std::vector<uint>* init_solution = nullptr;

    Instance::parseArgs(argc, argv, data_path, output_path, optimizer_type, config, seed, init_solution); 
    
    // Load instance
    wTSPSDInstance inst = wTSPSDInstance(data_path.c_str());

    BasicOptimizer* optimizer;
    if (optimizer_type == "local"){
        optimizer = new Optimizer(&inst, config, seed);
        if (init_solution != nullptr) optimizer->setInitSolution(*init_solution);
    }
    else if (optimizer_type == "evolutionary"){
        optimizer = new ASCHEA(&inst, config, seed);
    }

    std::cout << "Solving " << data_path << std::endl;
    optimizer->run();    

    Solution sol = optimizer->getSolution();

    // Export
    if (!output_path.empty()) {
        output_file.open(output_path);
        optimizer->saveToJson(output);
        wTSPSDInstance::export_perm_orig_ids(sol.permutation, output);
        inst.export_walk_orig_ids(sol.permutation, output);
        output_file << output.dump(4);
    } else {
        sol.print();
        std::cout << "fitness evals: " << inst.fitness_evals << std::endl;
        std::cout << sol.fitness << std::endl;
    }

    if (sol.is_feasible) {
        return 0;
    } else {
        return 1;
    }
}