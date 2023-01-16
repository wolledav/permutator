#include "config.hpp"

nlohmann::json Config::read_default_config(const std::string& problem_type) {
    return read_json("./configs/default_config.json")[problem_type];
}
