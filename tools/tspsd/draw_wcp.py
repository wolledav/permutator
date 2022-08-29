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
# for node1 in data['NODE_COORDS']:
#     for node2 in data['DELETE'][node1]:
#         x1, y1 = data['NODE_COORDS'][node1][0], data['NODE_COORDS'][node1][1]
#         x2, y2 = data['NODE_COORDS'][node2][0], data['NODE_COORDS'][node2][1]
#         Xs = [x1, x2]
#         Ys = [y1, y2]
#         # plt.plot(Xs, Ys, color='grey', alpha=0.2, linewidth=1)
# #        plt.plot(x1 + (x2 - x1)/4, y1 + (y2 - y1)/4, color='red', alpha=0.5, marker='x')
#         red_arrow = plt.arrow(x1, y1, (x2 - x1) / 4, (y2 - y1) / 4, color='red', alpha=0.5, head_width=1, linewidth=0.1)

# plot solution
if solution != '':
    with open(solution) as sol_file:
        sol_data = json.load(sol_file)
        walk = sol_data["solution"]["walk_orig_ids"]
        processing = sol_data["solution"]["walk_processing"]
        for i in range(len(walk)):
            node1 = walk[i]
            node2 = walk[(i + 1) % len(walk)]
            x1, y1 = data['NODE_COORDS'][node1][0], data['NODE_COORDS'][node1][1]
            x2, y2 = data['NODE_COORDS'][node2][0], data['NODE_COORDS'][node2][1]
            if processing[i] and processing[(i + 1) % len(walk)]:
                green_arrow = plt.arrow(x1, y1, (x2 - x1), (y2 - y1), color='green', alpha=0.75, head_width=8, linewidth=0.8, length_includes_head=True)
            else:
                orange_arrow = plt.arrow(x1, y1, (x2 - x1), (y2 - y1), color='orange', alpha=0.75, head_width=8, linewidth=0.8, length_includes_head=True)

        title += "\nfitness=" + str(sol_data["solution"]["fitness"]) + ", valid=" + str(sol_data["solution"]["is_feasible"])

if orange_arrow is not None and green_arrow is not None:
    plt.legend([orange_arrow, green_arrow], ['endpoint(s) not processed', 'endpoints processed'])
elif orange_arrow is not None and green_arrow is None:
    plt.legend([orange_arrow, ], ['endpoint(s) not processed', ])
elif orange_arrow is None and green_arrow is not None:
    plt.legend([green_arrow, ], ['endpoints processed', ])

plt.title(title)
plt.gca().set_aspect('equal', adjustable='box')

if output != '':
    fig_output = output + inp.split('/')[-1].split('.')[0]
    print(fig_output)
    plt.savefig(fig_output + '.pdf')
else:
    plt.show()
