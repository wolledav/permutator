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
STEP = 0
DECIMALS = 1

solutions = os.listdir(LOG_DIR)
deg2cnt = {} # avg_degree -> [feasible, total]

for solution in solutions:
    sol_data = get_data(LOG_DIR + solution)
    feasible = sol_data["solution"]["is_feasible"]
    name = sol_data["name"]
    problem_data = get_data(PROBLEM_DIR + name + ".json")
    exp_degrees = problem_data["EXP_DEGREES"]
    degree = round(exp_degrees[STEP], DECIMALS)
    if degree not in deg2cnt.keys():
        deg2cnt[degree] = [0, 0]
    if feasible:
        deg2cnt[degree][0] += 1
    deg2cnt[degree][1] += 1

degrees_sorted = sorted(deg2cnt.keys())
feasible_list = []
total_list = []
prob_list = []
for degree in degrees_sorted:
    feasible_list.append(deg2cnt[degree][0])
    total_list.append(deg2cnt[degree][1])
    prob = deg2cnt[degree][0]/deg2cnt[degree][1]
    prob_list.append(prob)

last_zero = degrees_sorted[0]
first_one = degrees_sorted[-1]
for x in zip(prob_list, degrees_sorted):
    if x[0] == 0.0:
        last_zero = x[1]
    if x[0] == 1.0 and x[1] < first_one:
        first_one = x[1]
    # print(x)

# for deg in degrees_sorted:
#     print(deg, deg2cnt[deg])

print("Feasible: " + str(sum(feasible_list)))
print("Total: " + str(sum(total_list)))
gap = first_one - last_zero
print("VD gap: " + str(gap))
# print(degrees_sorted)
# print(feasible_list)
# print(total_list)
# print(prob_list)

plt.plot(degrees_sorted, prob_list)
plt.title(PROBLEM_DIR + "\n uncertain VD gap: " + "{:.2f}".format(gap))
plt.xlabel("Expected average vertex degree after " + str(STEP) + " steps")
plt.ylabel("Hamiltonian - probability []")
plt.grid()

fig_output = FIG_DIR + LOG_DIR.split('/')[-2] + '_step_' + str(STEP) + '.pdf'
plt.savefig(fig_output)
print("Exported " + fig_output)

plt.show()

