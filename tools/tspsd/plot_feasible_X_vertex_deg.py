#!/usr/bin/python
import os
import json
import matplotlib.pyplot as plt


def get_data(input_dir):
    files = os.listdir(input_dir)
    deg2cnt = {} # avg_degree -> [feasible, total]
    for filename in sorted(files):
        path = input_dir + filename
        with open(path) as f:
            data = json.load(f)
            name = data["name"]
            feasible = data["solution"]["is_feasible"]
            parsed = name.split('-')
            avg_degree = float(parsed[2])
            if avg_degree not in deg2cnt.keys():
                deg2cnt[avg_degree] = [0, 0]
            if feasible:
                deg2cnt[avg_degree][0] += 1
            deg2cnt[avg_degree][1] += 1
    degrees_sorted = sorted(deg2cnt.keys())
    feasible_list = []
    total_list = []
    prob_list = []
    for degree in degrees_sorted:
        feasible_list.append(deg2cnt[degree][0])
        total_list.append(deg2cnt[degree][1])
        prob_list.append(deg2cnt[degree][0]/deg2cnt[degree][1])
    return degrees_sorted, prob_list


INPUT_DIRS = [
            "./log/scp-meta/random24_ov_250_SCP_3s/",
            # "./log/scp-meta/random24_ov_100_SCP_4s/"
            # "./log/scp-meta/random_tsp24_100_TSP_3s/",
            # "./log/wcp-meta/random24_ov_100_WCP_3s/"
            ]

OUTPUT_DIR = "./figures/scp/hamiltonicity/"

fig_output = OUTPUT_DIR
datasets = []
for input_dir in INPUT_DIRS:
    degrees, probs = get_data(input_dir)
    dataset = input_dir.split('/')[-2]
    datasets.append(dataset)
    plt.plot(degrees, probs)
    fig_output = fig_output + dataset + ","

fig_output = fig_output[:-1] + ".pdf"
plt.legend(datasets)

plt.grid()
plt.xlabel("Expected average vertex degree after n/2 steps")
plt.ylabel("Hamiltonian - probability []")

if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)
plt.savefig(fig_output)
print("Exported " + fig_output)

plt.show()
