#include "objective.hpp"

Objective::Objective() {
    this->mean_risk = 0.0;
    this->expected_excess = 0.0;
    this->final_objective = 0.0;
    this->total_resource_use = 0.0;
    this->workload_underuse = 0.0;
    this->workload_overuse = 0.0;
    this->exclusion_penalty = 0;
    this->extended_objective = 0.0;
}

void Objective::print_state() {
    printf("estimated mean risk: \t\t%f\n", this->mean_risk);
    printf("estimated expected excess: \t%f\n", this->expected_excess);
    printf("estimated final objective:  \t%f\n\n", this->final_objective);
    printf("estimated total workload use: \t%f\n", this->total_resource_use);
    printf("estimated workload overuse: \t%f\n", this->workload_overuse);
    printf("estimated workload underuse: \t%f\n", this->workload_underuse);
    // printf("\tWorkload usage: \t%f\n", this->workload_usage);
    printf("estimated exclusion penalty: \t%u\n", this->exclusion_penalty);
    printf("estimated extended objective: \t%f\n\n", this->extended_objective);
}

bool Objective::is_valid() {
    return this->workload_overuse == 0.0 &&
           this->workload_underuse == 0.0 &&
           this->exclusion_penalty == 0;
}
