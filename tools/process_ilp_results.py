# USAGE
# python3 ./tools/process_ilp_results.py ./log/cvrp-ilp/cvrp-10_results_10t/ CVRP
# python3 ./tools/process_ilp_results.py ./log/qap-ilp/qap-10_results/ QAP
# python3 ./tools/process_ilp_results.py ./log/npfs-ilp/npfs-10_results/ NPFS

import os
import sys

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
        "inst9x9_5_0": {"BKS": 0, "optimal": 0},
        "inst9x9_5_1": {"BKS": 0, "optimal": 0},
        "inst9x9_5_2": {"BKS": 0, "optimal": 0},
        "inst16x16_5_0": {"BKS": 0, "optimal": 0},
        "inst16x16_10_0": {"BKS": 0, "optimal": 0},
        "inst16x16_15_1": {"BKS": 0, "optimal": 0},
        "inst25x25_5_0": {"BKS": 0, "optimal": 0},
        "inst25x25_10_0": {"BKS": 0, "optimal": 0},
        "inst25x25_15_0": {"BKS": 0, "optimal": 0},
        "inst25x25_20_0": {"BKS": 0, "optimal": 0},
    },
}


def processFile(filename, dir, data):
    path = dir + filename
    file = open(path, "r")
    lines = [line.rstrip() for line in file]
    name = filename[4:-4]
    for line in lines:
        words = line.split(": ")
        if words[0] == "MIPGap":
            data[name]["MIPGap"] = words[1]
        elif words[0] == "Runtime":
            data[name]["runtime"] = words[1]
        elif words[0] == "Best solution":
            data[name]["fitness"] = words[1]
        elif words[0] == "Best bound":
            data[name]["LB"] = words[1]


def printLatex(data):
    for problem in data:
        print(problem, data[problem])


dir = sys.argv[1]
problem = sys.argv[2]

files = os.listdir(dir)
data = all_data[problem]

for filename in files:
    if filename[-4:] == ".out":
        processFile(filename, dir, data)

printLatex(data)
