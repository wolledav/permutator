#include "instance.hpp"

using namespace permutator;
using std::string;

Instance::Instance(const string& name, uint node_count) {
    this->name = name;
    this->node_cnt = node_count;
    this->lbs.resize(node_count);
    this->ubs.resize(node_count);
    for (uint i = 0; i < node_count; i++) {
        this->lbs[i] = 0;
        this->ubs[i] = std::numeric_limits<uint>::max()/2;
    }
}

Instance::Instance(const string& name, uint node_count, uint common_lb, uint common_ub) {
    this->name = name;
    this->node_cnt = node_count;
    this->lbs.resize(node_count);
    this->ubs.resize(node_count);
    for (uint i = 0; i < node_count; i++) {
        this->lbs[i] = common_lb;
        this->ubs[i] = common_ub;
    }
}

fitness_t Instance::getLBPenalty(const std::vector<uint> &frequency) {
    uint sum = 0;
    for (uint i = 0; i < this->node_cnt; i++) {
        sum += std::max((int)this->lbs[i] - (int)frequency[i], 0);
    }
    return sum;
}

bool Instance::FrequencyInBounds(const std::vector<uint> &frequency) {
    for (uint i = 0; i < this->node_cnt; i++){
        if (frequency[i] < this->lbs[i] || frequency[i] > this->ubs[i])
            return false;
    }
    return true;
}

void Instance::parseArgs(int argc, char *argv[], string &data_path, string &output_path, string &optimizer_type, nlohmann::json &config, uint& seed, std::vector<uint> *init_solution){

    uint timeout_s = UINT32_MAX;
    string conf_path, init_path;
    int opt;

    while ((opt = getopt(argc, argv, "led:t:s:o:c:")) != -1) {
        switch (opt) {
            case 'l':
                optimizer_type = "local";
                break;
            case 'e':
                optimizer_type = "evolutionary";
                break;
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

    if (!init_path.empty() && init_solution != nullptr) {
        std::cout << "Initial solution read from " + init_path << std::endl;
        nlohmann::json data = readJson(init_path);
        std::vector<uint> temp = data["solution"]["route"];
        if (std::find(temp.begin(), temp.end(), 0) == temp.end()) {
            std::cout << "Reindexing initial solution" << std::endl;
            for (auto &i:temp) i--;
        }
        *init_solution = temp;
    }

}
