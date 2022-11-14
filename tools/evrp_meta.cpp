#include <iostream>

#include "generic/LS_optimizer.hpp"
#include "problem/evrp.hpp"

using std::string;
using std::vector;

// TODO: setup argument parsing and link files



void show_usage()
{
    std::cout << "Usage: evrp_meta -d data_path [-c] config_path [-t] timeout(sec) [-s] seed [-o] output_file_path\n";
}

int main (int argc, char *argv[])
{
    uint seed = 0, timeout_s = 0;
    string data_path, output_path, conf_path;
    std::ofstream output_file, log_file;
    int opt;
    json config, output;
    // Parse arguments
    while ((opt = getopt(argc, argv, "d:t:s:o:c:")) != -1) {
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
            default: /* '?' */
                std::cout << "Unknown arg option: " << (char)opt << std::endl;
                show_usage();
                exit(EXIT_FAILURE);
        }
    }

    // Display usage instructions
    if (data_path.empty()) {
        show_usage();
        exit(EXIT_FAILURE);
    }

    // Load config
    if (!conf_path.empty()) {
        config = read_json(conf_path);
    } else {
        config = Config::read_default_config("evrp");
    }

    // Rewrite timeout
    if (timeout_s != 0) {
        config["timeout"] = timeout_s;
    }

    // Parse and solve instance
    string filename = get_filename(data_path);
    EVRPInstance inst = EVRPInstance(data_path.c_str());
    std::cout << "Solving " << inst.name << std::endl;
    LS_optimizer optimizer = LS_optimizer(&inst, config, seed);
    optimizer.run();

    // Export solution
    Solution sol = optimizer.getSolution();
    if (!output_path.empty()) {
        output_file.open(output_path);
        optimizer.save_to_json(output);
        output_file << output.dump(4);
    } else {
        std::cout << sol.fitness << std::endl;
    }
    inst.print_solution(&sol, std::cout);
    return 0;
} 