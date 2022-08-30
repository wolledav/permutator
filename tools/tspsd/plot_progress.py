#!/usr/bin/python

import sys
import os
import json
import matplotlib.pyplot as plt

LOG_DIR = sys.argv[1]

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
        timeout = data["config"]["timeout"]
        steps = [int(i) for i in data["steps"]]
        valid = data["solution"]["is_feasible"]

        m = max(steps)
        if valid:
            Xs.append(m)
            Ys.append(sd)
        else:
            Xs_inv.append(m)
            Ys_inv.append(sd)

        # for step in steps:
        #     if valid:
        #         Xs.append(int(step))
        #         Ys.append(int(sd))
        #     else:
        #         Xs_inv.append(int(step))
        #         Ys_inv.append(int(sd))

plt.plot(Xs, Ys, '.', color='green')
plt.plot(Xs_inv, Ys_inv, '.', color='red')

plt.show()
