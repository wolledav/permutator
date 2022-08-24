#include "config.hpp"

json Config::read_default_config(const string& problem_type) {
    return read_json("../configs/default_config.json")[problem_type];
}
