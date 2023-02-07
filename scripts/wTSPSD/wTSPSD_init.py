#!/usr/bin/python

import os
import subprocess
import time

CMD = "./cmake-build-release/wTSPSD_meta"
CONFIG = "./configs/TSPSD_config_opt_random.json"
PROBLEM_TYPE = "wTSPSD"

DATASET_DIR = "./data/tspsd/tsplib_10/selected/"
LOG_DIR = "./log/wTSPSD-meta/tsplib_10/selected_init/"
INIT_DIR = "./log/TSPSD-meta/tsplib_10/BKSs/"

PROBLEMS = ["burma14-3.1", "ulysses22-5.5", "berlin52-10.4", "berlin52-13.2", "eil101-27.5", "gr202-67.3", "lin318-99.3", "fl417-160.6", "d657-322.7", "rat783-481.4", "vm1084-848.9"]
TIMEOUTS = [140, 220, 520, 520, 1010, 2020, 3180, 4170, 6570, 7830, 10840]


SEED_FIRST = 0
SEED_LAST = 1


for seed in range(SEED_FIRST, SEED_LAST + 1):
    for problem, timeout in zip(PROBLEMS, TIMEOUTS):
        problem_path = DATASET_DIR + problem + ".json"
        log_path = LOG_DIR + problem + "_" + str(timeout) + "s/"
        if not os.path.exists(log_path):
            os.makedirs(log_path)
        log = log_path + str(seed) + "-" + problem + ".out"
        run = subprocess.run([CMD, "-d", problem_path, "-t", str(timeout), "-o", log, "-c", CONFIG, "-s", str(seed), "-i", INIT_DIR + problem + ".out"])
        time.sleep(5)
