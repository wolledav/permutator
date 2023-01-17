#include "config.hpp"

nlohmann::json Config::readDefaultConfig(const std::string& problem_type) {
    return readJson("./configs/default_config.json")[problem_type];
}
