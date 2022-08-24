#!/usr/bin/python

import sys
import json
import matplotlib.pyplot as plt

inp = sys.argv[1]

# Load .tsp instance from json
with open(inp) as input_file:
    data = json.load(input_file)

Xs = []
Ys = []

# Plot nodes
for node in data['NODE_COORDS']:
    Xs.append(data['NODE_COORDS'][node][0])
    Ys.append(data['NODE_COORDS'][node][1])
plt.plot(Xs, Ys, '.', color='black')

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
        plt.plot(Xs, Ys, color='grey', alpha=0.2, linewidth=1)
#        plt.plot(x1 + (x2 - x1)/4, y1 + (y2 - y1)/4, color='red', alpha=0.5, marker='x')
        arrow = plt.arrow(x1, y1, (x2 - x1)/4, (y2 - y1)/4, color='red', alpha=0.5, head_width=5, linewidth=1)

plt.title(inp.split('/')[-1])
plt.legend([arrow, ], ['deleted edges'])

plt.gca().set_aspect('equal', adjustable='box')

if len(sys.argv) > 2:
    fig_output = sys.argv[2] + inp.split('/')[-1].split('.')[0]
    plt.savefig(fig_output + '.pdf')
else:
    plt.show()
