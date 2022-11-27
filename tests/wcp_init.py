#!/usr/bin/python

import os
import subprocess
import time

CMD = "./cmake-build-release/wcp_meta"
CONFIG = "./configs/SCP_config_opt_random.json"
PROBLEM_TYPE = "WCP"

DATASET_DIR = "./data/tspsd/tsplib_10/selected/"
LOG_DIR = "./log/wcp-meta/tsplib_10/selected_init/"
INIT_DIR = "./log/scp-meta/tsplib_10/BKSs/"

PROBLEMS = ["burma14-2.20", "ulysses22-3.80", "berlin52-5.00", "berlin52-8.30", "eil101-18.00", "gr202-50.00", "lin318-70.00", "fl417-130.00", "d657-290.00", "rat783-460.00", "vm1084-840.00"]
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
