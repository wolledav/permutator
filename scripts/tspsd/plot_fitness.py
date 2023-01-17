#!/usr/bin/python

import sys
import os
import json
import matplotlib.pyplot as plt

LOG_DIR = ""
OUTPUT = ""
TYPE = ""

for i in range(len(sys.argv)):
    if sys.argv[i] == '-i':
        LOG_DIR = sys.argv[i+1]
    elif sys.argv[i] == '-o':
        OUTPUT = sys.argv[i+1]
    elif sys.argv[i] == '-t':
        TYPE = sys.argv[i+1]

logs = os.listdir(LOG_DIR)

Xs = []
Ys = []
Xs_inv = []
Ys_inv = []

for log in logs:
    inp = LOG_DIR + log
    sd = int(log.split('_')[-2][2:])
    with open(inp) as input_file:
        data = json.load(input_file)
        valid = data["solution"]["is_feasible"]
        fitness = data["solution"]["fitness"]

        if valid:
            Xs.append(sd)
            Ys.append(fitness)
        else:
            Xs_inv.append(sd)
            Ys_inv.append(0)

plt.plot(Xs, Ys, '.', color='green')
plt.plot(Xs_inv, Ys_inv, 'x', color='red')

plt.title(TYPE + ": " + LOG_DIR.split('/')[-2])
plt.xlabel('deleted edges per node')
plt.ylabel('fitness')
plt.legend(['valid', 'not valid'])
plt.grid()

if OUTPUT != '':
    fig_output = OUTPUT + LOG_DIR.split('/')[-2].split('.')[0] + '_fitness.pdf'
    plt.savefig(fig_output)
    print("Exported " + fig_output)
else:
    plt.show()
