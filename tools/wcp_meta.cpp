//
// Created by wolledav on 24.8.22.
//

#include <iostream>
#include <string>

#include "generic/LS_optimizer.hpp"
#include "problem/wcp.hpp"

using std::string;

void show_usage(){
    std::cout << "Usage: wcp_meta -d dataset_path [-t] timeout(sec) [-s] seed [-o] output file path [-i] initial solution path\n";
}

int main (int argc, char *argv[]) {
    uint seed = 0, timeout_s = 0;
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
        config = read_json(conf_path);
    } else {
        config = Config::read_default_config("wcp");
    }

    if (timeout_s != 0) {
        config["timeout"] = timeout_s;
    }

    // Load instance
    WCPInstance inst = WCPInstance(data_path.c_str());
    LS_optimizer optimizer = LS_optimizer(&inst, config, seed);

    if (!init_path.empty()) {
        std::cout << "Initial solution read from " + init_path << std::endl;
        json data = read_json(init_path);
        std::vector<uint> init_solution = data["solution"]["permutation"];
        optimizer.setInitSolution(init_solution);
    }

    std::cout << "Solving " << data_path << std::endl;
    optimizer.run();
    Solution sol = optimizer.getSolution();

    // Export
    if (!output_path.empty()) {
        output_file.open(output_path);
        optimizer.save_to_json(output);
        WCPInstance::export_perm_orig_ids(sol.permutation, output);
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