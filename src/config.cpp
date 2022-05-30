#include "config.hpp"

json Config::read_config(const string& path) {
    // read a JSON file
    json cfg;
    std::ifstream ifs(path, std::ifstream::in);
    if (!ifs)
        throw std::system_error(ENOENT, std::system_category(), path);
    ifs >> cfg;
    return cfg;
}

json Config::read_default_config(const string& problem_type) {
    return read_config("../data/default_config.json")[problem_type];
}
