#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

#include "generic/EA.hpp"
#include "src/problem/EVRP/EVRP.hpp"
#include "lib/getopt/getopt.h"

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

void show_usage(){
    std::cout << "Usage: EVRP_meta -d data_path [-c] config_path [-t] timeout(sec) [-s] seed [-o] output_file_path\n";
}

int main (int argc, char *argv[])
{
    uint customer_cnt, charger_cnt, tours, seed = 0;
    uint timeout_s = UINT32_MAX;
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
        config = readJson(conf_path);
    } else {
        config = Config::readDefaultConfig();
    }

    // Rewrite timeout
    if (timeout_s != UINT32_MAX) {
        config["timeout"] = timeout_s;
    }

    // Parse and solve instance
    string filename = getFilename(data_path);
    parse_filename(filename, &customer_cnt, &charger_cnt);
    EVRPInstance inst = EVRPInstance(data_path.c_str(), customer_cnt, charger_cnt);
    std::cout << "Solving " << inst.name << std::endl;
    EA optimizer = EA(&inst, config, seed);
    optimizer.run();

    // Export solution
    Solution sol = optimizer.getSolution();
    if (!output_path.empty()) {
        output_file.open(output_path);
        optimizer.saveToJson(output);
        output_file << output.dump(4);
    } else {
        inst.print_solution(&sol, std::cout);
        std::cout << sol.fitness << std::endl;
    }
    return 0;
} 