#!/usr/bin/python

import sys
import json
import matplotlib.pyplot as plt

inp = ""
output = ""
solution = ""

red_arrow = None
green_arrow = None

for i in range(len(sys.argv)):
    if sys.argv[i] == '-i':
        inp = sys.argv[i+1]
    elif sys.argv[i] == '-o':
        output = sys.argv[i+1]
    elif sys.argv[i] == '-s':
        solution = sys.argv[i+1]

# Load .tsp instance from json
title = inp.split('/')[-1]
with open(inp) as input_file:
    data = json.load(input_file)

Xs = []
Ys = []

# Plot nodes
for node in data['NODE_COORDS']:
    Xs.append(data['NODE_COORDS'][node][0])
    Ys.append(data['NODE_COORDS'][node][1])
plt.plot(Xs, Ys, '.', color='black', markersize=2)

# Plot complete graph
# for node1 in data['NODE_COORDS']:
#     for node2 in data['NODE_COORDS']:
#        Xs = [data['NODE_COORDS'][node1][0], data['NODE_COORDS'][node2][0]]
#        Ys = [data['NODE_COORDS'][node1][1], data['NODE_COORDS'][node2][1]]
#        plt.plot(Xs, Ys, color='grey', alpha=0.05, linewidth=1)

# Plot delete function
for node1 in data['NODE_COORDS']:
    for node2 in data['DELETE'][node1]:
        x1, y1 = data['NODE_COORDS'][node1][0], data['NODE_COORDS'][node1][1]
        x2, y2 = data['NODE_COORDS'][node2][0], data['NODE_COORDS'][node2][1]
        Xs = [x1, x2]
        Ys = [y1, y2]
        # plt.plot(Xs, Ys, color='grey', alpha=0.2, linewidth=1)
#        plt.plot(x1 + (x2 - x1)/4, y1 + (y2 - y1)/4, color='red', alpha=0.5, marker='x')
        red_arrow = plt.arrow(x1, y1, (x2 - x1) / 4, (y2 - y1) / 4, color='red', alpha=0.5, head_width=10, linewidth=1)

# plot solution
if solution != '':
    with open(solution) as sol_file:
        sol_data = json.load(sol_file)
        permutation = sol_data["solution"]["permutation_orig_ids"]
        for i in range(len(permutation)):
            node1 = permutation[i]
            node2 = permutation[(i + 1) % len(permutation)]
            x1, y1 = data['NODE_COORDS'][node1][0], data['NODE_COORDS'][node1][1]
            x2, y2 = data['NODE_COORDS'][node2][0], data['NODE_COORDS'][node2][1]
            green_arrow = plt.arrow(x1, y1, (x2 - x1), (y2 - y1), color='green', alpha=0.75, head_width=10, linewidth=1, length_includes_head=True)
        title += "\nfitness=" + str(sol_data["solution"]["fitness"]) + ", valid=" + str(sol_data["solution"]["is_feasible"])

if red_arrow is not None and green_arrow is not None:
    plt.legend([red_arrow, green_arrow], ['deleted edges', 'traversed edges'])
elif red_arrow is not None and green_arrow is None:
    plt.legend([red_arrow, ], ['deleted edges', ])
elif red_arrow is None and green_arrow is not None:
    plt.legend([green_arrow, ], ['traversed edges', ])

plt.title(title)
plt.gca().set_aspect('equal', adjustable='box')

if output != '':
    fig_output = output + inp.split('/')[-1].split('.')[0]
    print(fig_output)
    plt.savefig(fig_output + '.pdf')
else:
    plt.show()
