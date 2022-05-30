//
// Created by honza on 11.03.22.
//
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

#include "generic/pwr_optimizer.hpp"
#include "problem/sudoku.hpp"

using std::string;
using std::vector;

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
    std::cout << "Usage: qap_meta -d dataset_path [-t] timeout(sec) [-s] seed [-o] output file path\n";
}

int main (int argc, char *argv[])
{
    uint node_cnt, seed = 0, timeout_s = 0;
    string data_path, output_path, conf_path;
    std::ofstream log_file, output_file;
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
    if (data_path.empty()) {
        show_usage();
        exit(EXIT_FAILURE);
    }

    if (!conf_path.empty()) {
        config = Config::read_config(conf_path);
    } else {
        config = Config::read_default_config("sudoku");
    }

    if (timeout_s != 0) {
        config["timeout"] = timeout_s;
    }

    string filename = get_filename(data_path);
    parse_filename(filename, &node_cnt);
    SudokuInstance inst = SudokuInstance(data_path, node_cnt);
    std::cout << "Solving " << inst.name << std::endl;
    PWROptimizer optimizer = PWROptimizer(&inst, config, seed);
    optimizer.run();
    Solution sol = optimizer.getSolution();
    if (!output_path.empty()) {
        output_file.open(output_path);
        optimizer.save_to_json(output);
        output_file << output.dump(4);
    } else {
        inst.print_solution(&sol, std::cout);
    }
    return 0;
}

