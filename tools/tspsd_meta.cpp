//
// Created by wolledav on 24.8.22.
//

#include <iostream>
#include <string>

#include "generic/LS_optimizer.hpp"
#include "problem/tspsd.hpp"

using std::string;

void show_usage(){
    std::cout << "Usage: tspsd_meta -d dataset_path [-t] timeout(sec) [-s] seed [-o] output file path\n";
}

int main (int argc, char *argv[]) {
    uint seed = 0, timeout_s = 0;
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

    // Treat empty -d, -c, -t
    if (data_path.empty()) {
        show_usage();
        exit(EXIT_FAILURE);
    }

    if (!conf_path.empty()) {
        config = Config::read_config(conf_path);
    } else {
        config = Config::read_default_config("tspsd");
    }

    if (timeout_s != 0) {
        config["timeout"] = timeout_s;
    }

    // Load instance
    TSPSDInstance inst = TSPSDInstance(data_path.c_str());


}