# USAGE
# python3 ./tools/process_meta_results.py ./log/cvrp-meta/test/ CVRP

import os
import sys
import json
import numpy as np


all_data = {
    "CVRP": {
        "P-n050-k10": {"BKS": 696, "optimal": 1},
        "A-n65-k09": {"BKS": 1174, "optimal": 1},
        "P-n076-k05": {"BKS": 627, "optimal": 1},
        "X-n148-k46": {"BKS": 43448, "optimal": 1},
        "X-n204-k19": {"BKS": 19565, "optimal": 1},
        "X-n251-k28": {"BKS": 38684, "optimal": 1},
        "X-n351-k40": {"BKS": 25896, "optimal": 0},
        "X-n561-k42": {"BKS": 42717, "optimal": 0},
        "X-n749-k98": {"BKS": 77269, "optimal": 0},
        "X-n1001-k43": {"BKS": 72355, "optimal": 0},
    },
    "QAP":  {
        "tai20b": {"BKS": 122455319, "optimal": 1},
        "tai25a": {"BKS": 1167256, "optimal": 1},
        "tai30b": {"BKS": 637117113, "optimal": 1},
        "tai40a": {"BKS": 3139370, "optimal": 0},
        "tai50b": {"BKS": 458821517, "optimal": 0},
        "tai60a": {"BKS": 7205962, "optimal": 0},
        "tai80a": {"BKS": 13499184, "optimal": 0},
        "tai80b": {"BKS": 818415043, "optimal": 0},
        "tai100a": {"BKS": 21052466, "optimal": 0},
        "tai100b": {"BKS": 1185996137, "optimal": 0},
    },
    "NPFS": {
        "VFR20_5_1": {"BKS": 1192, "optimal": 0},
        "VFR20_15_1": {"BKS": 1936, "optimal": 0},
        "VFR40_5_1": {"BKS": 2396, "optimal": 0},
        "VFR100_20_1": {"BKS": 6172, "optimal": 0},
        "VFR100_20_2": {"BKS": 6267, "optimal": 0},
        "VFR200_20_1": {"BKS": 11272, "optimal": 0},
        "VFR400_40_1": {"BKS": 23393, "optimal": 0},
        "VFR400_60_1": {"BKS": 25395, "optimal": 0},
        "VFR600_40_1": {"BKS": 33683, "optimal": 0},
        "VFR600_60_1": {"BKS": 35976, "optimal": 0},
    },
    "SUDOKU": {
        "inst9x9_5_0": {"BKS": 0, "optimal": 1},
        "inst9x9_5_1": {"BKS": 0, "optimal": 1},
        "inst9x9_5_2": {"BKS": 0, "optimal": 1},
        "inst16x16_5_0": {"BKS": 0, "optimal": 1},
        "inst16x16_10_0": {"BKS": 0, "optimal": 1},
        "inst16x16_15_1": {"BKS": 0, "optimal": 1},
        "inst25x25_5_0": {"BKS": 0, "optimal": 1},
        "inst25x25_10_0": {"BKS": 0, "optimal": 1},
        "inst25x25_15_0": {"BKS": 0, "optimal": 1},
        "inst25x25_20_0": {"BKS": 0, "optimal": 1},
    },
}


def processInstance(dir, data):
    fitness_all = []
    timeout = -1
    files = os.listdir(dir)
    instance = dir.split("/")[-2]

    for file in files:
        f = open(dir+file)
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
            print("%s & %.2f & %.2f & %.2f%% \\\\" % (instance, mean_fitness, stdev, GAP_BKS))
        else:
            print("%s & & &  \\\\" % instance)


top_dir = sys.argv[1]
dirs = os.listdir(top_dir)
problem = sys.argv[2]

data = all_data[problem]
for dir in dirs:
    processInstance(top_dir + dir + "/", data)

printLatex(data)
