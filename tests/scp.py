#!/usr/bin/python

import os
import subprocess

CMD = "./cmake-build-release/scp_meta_stable"
CONFIG = "./configs/SCP_config_opt.json"
PROBLEM_TYPE = "SCP"

DATASET_DIR = "./data/tspsd/tsplib_10/selected/"
LOG_DIR = "./log/scp-meta/tsplib_10/selected/"
PROBLEMS = ["burma14-2.20", "ulysses22-3.80", "berlin52-8.30", "eil101-18.00", "gr202-50.00", "lin318-70.00", "fl417-130.00", "d657-290.00"]
TIMEOUTS = [140, 220, 520, 1010, 2020, 3180, 4170, 6570]
CNT = 50

for problem, timeout in zip(PROBLEMS, TIMEOUTS):
    problem_path = DATASET_DIR + problem + ".json"
    log_path = LOG_DIR + problem + "_" + str(timeout) + "s/"
    if not os.path.exists(log_path):
        os.makedirs(log_path)
    for seed in range(CNT):
        log = log_path + str(seed) + "-" + problem + ".out"
        run = subprocess.run([CMD, "-d", problem_path, "-t", str(timeout), "-o", log, "-c", CONFIG, "-s", str(seed)])
