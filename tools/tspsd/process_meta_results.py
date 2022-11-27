#!/usr/bin/python
import os
from tspsd_common import *
import numpy as np

LOG_DIR = "./log/scp-meta/tsplib_10/selected/berlin52-8.30_520s/"


feasible_cnt = 0
scores = []
times = []

files = os.listdir(LOG_DIR)
for file in files:
    path = LOG_DIR + file
    data = get_data(path)
    feasible = data["solution"]["is_feasible"]
    score = data["solution"]["fitness"]
    time = max([int(k) for k in data["steps"].keys()])
    if feasible:
        feasible_cnt += 1
        scores.append(score)
        times.append(time)

total_cnt = len(files)
best = min(scores)
avg = np.mean(scores)
stdev = np.std(scores)
time = np.mean(times)
success = 100*feasible_cnt/total_cnt


print(best, avg, stdev, time, success)
output = " {0:.0f}\t\t& {1:.2f}\t\t& {2:.2f}\t\t& {3:.0f}\t\t\t\t& {4:.0f}\t\t\t\t".format(best, avg, stdev, time, success)
print(output)
