#pragma once

#include <string>
#include <fstream>
#include <utility>

#include "lib/nlohmann/json.hpp"
#include "src/utils.hpp"

// Compile time config macros
#define GUROBI_THREAD_CNT 1

#define NDEBUG

#ifdef NDEBUG
#define STDOUT_ENABLED 0
#else
#define STDOUT_ENABLED 1
#endif

namespace permutator {
    typedef unsigned int uint_t;
#ifdef FITNESS_TYPE
    typedef FITNESS_TYPE fitness_t;
#else
#define FITNESS_TYPE fitness_t
    typedef unsigned long int fitness_t;
#endif
}
#define UNIMPROVING_ITERS_MAX 10
#define LB_PENALTY 1000000000000

class Config {
private:
public:
    explicit Config() = default;
    static nlohmann::json readDefaultConfig();
};
