#!/usr/bin/python
import os
from tspsd_common import *
import numpy as np



LOG_DIRS = "./log/wTSPSD-meta/tsplib_10/selected_init/"
# LOG_DIR = "./log/wTSPSD-meta/tsplib_10/selected_init/vm1084-848.9_10840s/"

dirs = os.listdir(LOG_DIRS)
for dir in dirs:
    dir_path = LOG_DIRS + dir + "/"

    feasible_cnt = 0
    scores = []
    times = []

    best_score = float('inf')
    best_solution = ""

    files = os.listdir(dir_path)
    for file in files:
        path = dir_path + file
        data = get_data(path)
        feasible = data["solution"]["is_feasible"]
        score = data["solution"]["fitness"]
        time = max([int(k) for k in data["steps"].keys()])
        if feasible:
            feasible_cnt += 1
            scores.append(score)
            times.append(time)
        if score < best_score:
            best_score = score
            best_solution = path

    total_cnt = len(files)
    best = min(scores)
    avg = np.mean(scores)
    stdev = np.std(scores)
    time = np.mean(times)
    success = 100*feasible_cnt/total_cnt

    # print(best, avg, stdev, time, success)
    print(dir)
    output = " {0:.0f}\t\t& {1:.2f}\t\t& {2:.2f}\t\t& {3:.0f}\t\t\t\t& {4:.0f}\t\t\t".format(best, avg, stdev, time, success)
    print(output)
    print("best score: " + str(best_score) + ", file: " + best_solution)
    print()