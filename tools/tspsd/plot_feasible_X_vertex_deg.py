#!/usr/bin/python
import os
import json
import matplotlib.pyplot as plt


INPUT_DIR = "./log/scp-meta/random24_v2_3s/"
OUTPUT_DIR = "./figures/scp/hamiltonicity/"

files = os.listdir(INPUT_DIR)
deg2cnt = {} # avg_degree -> [feasible, total]

for filename in sorted(files):
    path = INPUT_DIR + filename
    with open(path) as f:
        data = json.load(f)
        name = data["name"]
        feasible = data["solution"]["is_feasible"]
        parsed = name.split('-')
        print(name)
        vertex_cnt = int(parsed[1])
        edges_removed = float(parsed[2])
        edges_total = int(vertex_cnt * (vertex_cnt - 1)/2)
        edges_cnt = edges_total - edges_removed
        avg_degree = 2 * edges_cnt / vertex_cnt
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

dataset = INPUT_DIR.split('/')[-2]

print(degrees_sorted)
print(prob_list)

plt.grid()
plt.xlabel("Average vertex degree")
plt.ylabel("Hamiltonian - probability []")
plt.title(dataset)
plt.plot(degrees_sorted, prob_list)

if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)
fig_output = OUTPUT_DIR + dataset + ".pdf"
plt.savefig(fig_output)
print("Exported " + fig_output)

plt.show()
