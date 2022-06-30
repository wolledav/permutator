# USAGE
# python3 ./tools/process_meta_results_v2.py ./log/cvrp-meta/test/ ./data/cvrp/cvrp-10_BKS_file.json

import os
import sys
import json
import numpy as np


def processInstance(log_dir, data):
    fitness_all = []
    timeout = -1
    files = os.listdir(log_dir)
    instance = log_dir.split("/")[-2]

    for file in files:
        f = open(log_dir+file)
        results = json.load(f)
        fitness_all.append(results["solution"]["solution"]["fitness"])
        timeout = results["timeout"]

    data[instance]["runtime"] = timeout
    data[instance]["mean_fitness"] = np.mean(fitness_all)
    data[instance]["stdev"] = np.std(fitness_all)


def printLatex(data):
    for instance in data:
        keys = data[instance].keys()
        if "mean_fitness" in keys and "stdev" in keys and "BKS" in keys:
            mean_fitness = data[instance]["mean_fitness"]
            stdev = data[instance]["stdev"]
            BKS = data[instance]["BKS"]
            if BKS != 0:
                GAP_BKS = 100 * (mean_fitness - BKS)/BKS
            else:
                GAP_BKS = -1000
            print("%s\t\t& %.2f\t\t& %.2f\t\t& %.2f%%\t\t\\\\" % (instance, mean_fitness, stdev, GAP_BKS))
        else:
            print("%s & & & \\\\" % instance)


top_dir = sys.argv[1]
BKS_file = sys.argv[2]

log_dirs = os.listdir(top_dir)

f = open(BKS_file)
data = json.load(f)

for instance in log_dirs:
    processInstance(top_dir + instance + "/", data)

printLatex(data)
