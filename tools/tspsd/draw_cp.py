#!/usr/bin/python

import sys
import json
import matplotlib.pyplot as plt

inp = ""
output = ""
solution = ""

red_label = None
green_label = None
black_label = None
blue_label = None

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

x_max = max(data['NODE_COORDS'].values())
factor = x_max[0]/1740.0

# Plot delete function
for node in data['NODE_COORDS']:
    for edge in data['DELETE'][node]:
        node1 = edge[0]
        node2 = edge[1]
        x1, y1 = data['NODE_COORDS'][node1][0], data['NODE_COORDS'][node1][1]
        x2, y2 = data['NODE_COORDS'][node2][0], data['NODE_COORDS'][node2][1]
        xm = (x1 + x2)/2
        ym = (y1 + y2)/2
        vertex = data['NODE_COORDS'][node]
        plt.plot([vertex[0], xm], [vertex[1], ym], color='red', alpha=0.3, linewidth=1, linestyle=':')
        ratio = 8
        plt.arrow(xm, ym, (x2 - xm) / ratio, (y2 - ym) / ratio, color='red', alpha=0.5, linewidth=1)
        # red_label = plt.arrow(xm, ym, (x1 - xm) / ratio, (y1 - ym) / ratio, color='red', alpha=0.5, linewidth=1)
        plt.arrow(xm, ym, (x2 - xm) / ratio, (y2 - ym) / ratio, color='red', alpha=0.5, linewidth=1, head_width=5 * factor)
        red_label = plt.arrow(xm, ym, (x1 - xm) / ratio, (y1 - ym) / ratio, color='red', alpha=0.5, linewidth=1, head_width=5 * factor)

# plot solution
if solution != '':
    with open(solution) as sol_file:
        sol_data = json.load(sol_file)
        walk = sol_data["solution"]["permutation_orig_ids"]
        processing = [True for i in range(len(walk))]
        if "walk_orig_ids" in sol_data["solution"]:
            walk = sol_data["solution"]["walk_orig_ids"]
            processing = sol_data["solution"]["walk_processing"]

        for i in range(len(walk)):
            node1 = walk[i]
            node2 = walk[(i + 1) % len(walk)]
            x1, y1 = data['NODE_COORDS'][node1][0], data['NODE_COORDS'][node1][1]
            x2, y2 = data['NODE_COORDS'][node2][0], data['NODE_COORDS'][node2][1]
            green_label = plt.arrow(x1, y1, (x2 - x1), (y2 - y1), color='green', alpha=0.5, linewidth=2, length_includes_head=True, head_width=8 * factor)
            # green_label = plt.arrow(x1, y1, (x2 - x1), (y2 - y1), color='green', alpha=0.5, linewidth=2, length_includes_head=True)
            if processing[i]:
                black_label = plt.arrow(x1, y1, (x2 - x1), (y2 - y1), color='black', alpha=1, head_width=0, linewidth=0, length_includes_head=True)
                plt.text(x1 + 1, y1 + 1, str(i), color='black', fontsize=8)
            else:
                blue_label = plt.arrow(x1, y1, (x2 - x1), (y2 - y1), color='blue', alpha=1, head_width=0, linewidth=0, length_includes_head=True)
                plt.text(x1 + 1, y1 - 16, str(i), color='blue', fontsize=8)

        title += "\nfitness=" + str(sol_data["solution"]["fitness"]) + ", valid=" + str(sol_data["solution"]["is_feasible"])

# Plot nodes
for node in data['NODE_COORDS']:
    Xs.append(data['NODE_COORDS'][node][0])
    Ys.append(data['NODE_COORDS'][node][1])
plt.plot(Xs, Ys, '.', color='black', markersize=3)

legend = []
legend_text = []
if red_label is not None:
    legend.append(red_label)
    legend_text.append("removed edges")
if green_label is not None:
    legend.append(green_label)
    legend_text.append("traversed edges")
if black_label is not None:
    legend.append(black_label)
    legend_text.append("processed nodes")
if blue_label is not None:
    legend.append(blue_label)
    legend_text.append("not processed nodes")

plt.legend(legend, legend_text)

plt.title(title)
plt.gca().set_aspect('equal', adjustable='box')
figsize = plt.rcParams['figure.figsize']
plt.gcf().set_size_inches(2*figsize[0], 2*figsize[1])

if output != '':
    fig_output = output + inp.split('/')[-1].split('.')[0]
    print("Exported " + fig_output)
    plt.savefig(fig_output + '.pdf')
else:
    plt.show()
