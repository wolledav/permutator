#include "config.hpp"

nlohmann::json Config::readDefaultConfig() {
    return readJson("./configs/default.json");
}
