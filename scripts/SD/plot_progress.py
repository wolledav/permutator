#!/usr/bin/python

import sys
import os
import json
import matplotlib.pyplot as plt

LOG = ""
OUTPUT_DIR = ""
TYPE = ""

for i in range(len(sys.argv)):
    if sys.argv[i] == '-i':
        LOG = sys.argv[i + 1]
    elif sys.argv[i] == '-o':
        OUTPUT_DIR = sys.argv[i + 1]
    elif sys.argv[i] == '-t':
        TYPE = sys.argv[i+1]

Xs = []
Ys = []

with open(LOG) as input_file:
    data = json.load(input_file)
    steps = data["steps"]
    timeout = data["timeout"]
    valid = data["solution"]["is_feasible"]
    for step in steps:
        Xs.append(int(step))
        Ys.append(int(steps[step]))

best = min(Ys)

Z = [x for _,x in sorted(zip(Xs,Ys))]
Xs = sorted(Xs)
Ys = Z

plt.plot(Xs, Ys, linestyle='-', marker='.', color='green')
plt.plot([timeout], [best], 'x', color='red')
plt.grid()
plt.title(TYPE + ": " + LOG.split('/')[-1] + '\n' + 'valid: ' + str(valid))
plt.xlabel('time [s]')
plt.ylabel('fitness')
plt.gcf().set_size_inches(2*6.4, 2*4.8)
plt.legend(['improving ILS iteration', 'timeout'])

current_values = plt.gca().get_yticks()
plt.gca().set_yticklabels(['{:.0f}'.format(x) for x in current_values])


if OUTPUT_DIR != '':
    fig_output = OUTPUT_DIR + LOG.split('/')[-1].split('.')[0] + '_progress.pdf'
    plt.savefig(fig_output)
    print("Exported " + fig_output)
else:
    plt.show()


