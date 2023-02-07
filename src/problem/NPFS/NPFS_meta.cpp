#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

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

void show_usage(){
    std::cout << "Usage: NPFS_meta -d dataset_path [-t] timeout(sec) [-s] seed [-o] output file path\n";
}

int main (int argc, char *argv[])
{
    uint job_cnt, machine_cnt, seed = 0, timeout_s = 0;
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
    if (data_path.empty()) {
        show_usage();
        exit(EXIT_FAILURE);
    }

    if (!conf_path.empty()) {
        config = readJson(conf_path);
    } else {
        config = Config::readDefaultConfig("NPFS");
    }

    if (timeout_s != 0) {
        config["timeout"] = timeout_s;
    }

    string filename = getFilename(data_path);
    parse_filename(filename, &job_cnt, &machine_cnt);
    NPFSInstance inst = NPFSInstance(data_path, job_cnt, machine_cnt);
    std::cout << "Solving " << inst.name << std::endl;
    Optimizer optimizer = Optimizer(&inst, config, seed);
    optimizer.run();
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