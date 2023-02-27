#include <iostream>
#include <string>

#include "generic/EA.hpp"
#include "src/problem/wTSPSD/wTSPSD.hpp"
#include "lib/getopt/getopt.h"

using std::string;
using nlohmann::json;

void show_usage(){
    std::cout << "Usage: wTSPSD_meta -d dataset_path [-t] timeout(sec) [-s] seed [-o] output file path [-i] initial solution path\n";
}

int main (int argc, char *argv[]) {
    uint seed = 0;
    uint timeout_s = UINT32_MAX;
    string data_path, output_path, conf_path, init_path;
    std::ofstream log_file, output_file;
    int opt;
    json config, output;

    // Parse arguments
    while ((opt = getopt(argc, argv, "d:t:s:o:c:i:")) != -1) {
        switch (opt) {
            case 'd':
                data_path = optarg;
                break;
            case 't':
                timeout_s = strtoul(optarg, nullptr, 10);
                break;
            case 's':
                seed = strtoul(optarg, nullptr, 10);
                break;
            case 'o':
                output_path = optarg;
                break;
            case 'c':
                conf_path = optarg;
                break;
            case 'i':
                init_path = optarg;
                break;
            default: /* '?' */
                std::cout << "Unknown arg option: " << (char)opt << std::endl;
                show_usage();
                exit(EXIT_FAILURE);
        }
    }

    // Treat empty -d, -c, -t
    if (data_path.empty()) {
        show_usage();
        exit(EXIT_FAILURE);
    }

    if (!conf_path.empty()) {
        config = readJson(conf_path);
    } else {
        config = Config::readDefaultConfig();
    }

    if (timeout_s != UINT32_MAX) {
        config["timeout"] = timeout_s;
    }

    // Load instance
    wTSPSDInstance inst = wTSPSDInstance(data_path.c_str());
    EA optimizer = EA(&inst, config, seed);

    if (!init_path.empty()) {
        std::cout << "Initial solution read from " + init_path << std::endl;
        json data = readJson(init_path);
        std::vector<uint> init_solution = data["solution"]["permutation"];
        if (std::find(init_solution.begin(), init_solution.end(), 0) == init_solution.end()) {
            std::cout << "Reindexing initial solution" << std::endl;
            for (auto &i:init_solution) i--;
        }
        //optimizer.setInitSolution(init_solution);
    }

    std::cout << "Solving " << data_path << std::endl;
    optimizer.run();
    Solution sol = optimizer.getSolution();

    // Export
    if (!output_path.empty()) {
        output_file.open(output_path);
        optimizer.saveToJson(output);
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