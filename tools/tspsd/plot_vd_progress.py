#!/usr/bin/python

import os
import json
import matplotlib.pyplot as plt


def get_data(json_path):
    with open(json_path) as f:
        data = json.load(f)
        return data


PROBLEM_DIR = "./data/tspsd/random24_ov_100/"
LOG_DIR = "./log/scp-meta/random24_ov_100_SCP_3s/"
FIG_DIR = "./figures/scp/hamiltonicity/"

vd_prog_feasible = [0.0] * 25
feasible_cnt = 0
vd_prog_infeasible = [0.0] * 25
infeasible_cnt = 0
steps = []


problems = os.listdir(PROBLEM_DIR)
for problem in problems:
    # vd = float(problem.split('-')[-2])
    # if vd == VD:
    problem_data = get_data(PROBLEM_DIR + problem)
    name = problem_data['NAME']
    solution_data = get_data(LOG_DIR + name + ".out")
    feasible = solution_data["solution"]["is_feasible"]
    vd_progress = problem_data["EXP_DEGREES"]
    steps = [*range(problem_data["DIMENSION"] + 1)]
    if feasible:
        plt.plot(steps, vd_progress, '.', color='blue')
        vd_prog_feasible = [sum(x) for x in zip(vd_prog_feasible, vd_progress)]
        feasible_cnt += 1
    else:
        plt.plot(steps, vd_progress, 'x', color='red')
        vd_prog_infeasible = [sum(x) for x in zip(vd_prog_infeasible, vd_progress)]
        infeasible_cnt += 1

vd_prog_feasible[:] = [x / feasible_cnt for x in vd_prog_feasible]
vd_prog_infeasible[:] = [x / infeasible_cnt for x in vd_prog_infeasible]
h1 = plt.plot(steps, vd_prog_feasible, color='blue', label='feasible')
h2 = plt.plot(steps, vd_prog_infeasible, color='red', label='infeasible')

plt.legend()
plt.title("Instances " + PROBLEM_DIR)
plt.xlabel("Solution step")
plt.ylabel("Expected vertex degree")
plt.grid()
plt.show()

fig_output = FIG_DIR + LOG_DIR.split('/')[-2] + '_vd_progress.pdf'
plt.savefig(fig_output)
print("Exported " + fig_output)
