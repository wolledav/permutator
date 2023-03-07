#pragma once

#include <chrono>

#include "instance.hpp"
#include "solution.hpp"
#include "utils.hpp"


class BasicOptimizer
{
    protected:
        nlohmann::json config;
        uint timeout_s;
        Solution best_known_solution;
        std::chrono::steady_clock::time_point start, last_improvement;
        Instance* instance;

        long getRuntime() {return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - this->start).count();};
        virtual bool stop() = 0;

    public:
        virtual ~BasicOptimizer() {};
        virtual void run() {};
        virtual Solution getSolution() {return this->best_known_solution;};
        virtual void saveToJson(nlohmann::json& container) {};
        virtual void setInitSolution(std::vector<uint> init_solution) {};
};