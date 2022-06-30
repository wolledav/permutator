# USAGE
# python3 ./tools/process_ilp_results_v2.py ./log/cvrp-ilp/cvrp-10_results/ ./data/cvrp/cvrp-10_BKSs.json
# python3 ./tools/process_ilp_results_v2.py ./log/npfs-ilp/npfs-10_results/ ./data/npfs-vfr/npfs-10_BKSs.json
# python3 ./tools/process_ilp_results_v2.py ./log/qap-ilp/qap-10_results/ ./data/qap/qap-10_BKSs.json
# python3 ./tools/process_ilp_results_v2.py ./log/sudoku-ilp/sudoku-10_results/ ./data/sudoku/sudoku-10_BKSs.json

import os
import sys
import json


def processLog(log, dir, data):
    path = dir + log
    file = open(path, "r")
    lines = [line.rstrip() for line in file]
    name = log[4:-4]
    for line in lines:
        words = line.split(": ")
        if words[0] == "MIPGap":
            data[name]["MIPGap"] = float(words[1])
        elif words[0] == "Runtime":
            data[name]["runtime"] = float(words[1])
        elif words[0] == "Best solution":
            data[name]["fitness"] = int(words[1])
        elif words[0] == "Best bound":
            data[name]["LB"] = int(words[1])


def printLatex(data):
    for instance in data:
        keys = data[instance].keys()
        if "fitness" in keys and "LB" in keys and "BKS" in keys:
            fitness = data[instance]["fitness"]
            LB = data[instance]["LB"]
            BKS = data[instance]["BKS"]
            if BKS != 0:
                GAP_BKS = 100 * (fitness - BKS)/BKS
            else:
                GAP_BKS = -1000
            print("%s\t\t& %d\t\t& %d\t\t& %.2f%%\t\t\\\\" % (instance, fitness, LB, GAP_BKS))
        else:
            print("%s & & &  \\\\" % instance)


log_dir = sys.argv[1]
BKS_file = sys.argv[2]

logs = os.listdir(log_dir)

f = open(BKS_file)
data = json.load(f)

for log in logs:
    if log[-4:] == ".out":
        processLog(log, log_dir, data)

printLatex(data)
