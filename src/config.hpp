//
// Created by honza on 06.03.22.
//
#pragma once

#include <string>
#include <fstream>
#include <utility>

#include "lib/nlohmann/json.hpp"

// Compile time config macros
#define GUROBI_THREAD_CNT 8
#define STDOUT_ENABLED 0
namespace metaopt {
    typedef unsigned int uint;
#ifdef FITNESS_TYPE
    typedef FITNESS_TYPE fitness_t;
#else
#define FITNESS_TYPE fitness_t
    typedef unsigned int fitness_t;
#endif
}


using json = nlohmann::json;
using string = std::string;

class Config {
private:
public:
    explicit Config() = default;
    static json read_config(const string& path);
    static json read_default_config(const string& problem_type);
};
