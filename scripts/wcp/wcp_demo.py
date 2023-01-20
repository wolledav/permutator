#!/usr/bin/python

import os
import subprocess
import time

CMD = "./cmake-build-release/wcp_meta"
CONFIG = "./configs/SCP_config_opt_random.json"
PROBLEM_TYPE = "WCP"

DATASET_DIR = "./wcp_demo/"
LOG_DIR = "./wcp_demo/"

# PROBLEMS = ["burma14-2.20", "ulysses22-3.80", "berlin52-5.00", "berlin52-8.30", "eil101-18.00", "gr202-50.00", "lin318-70.00", "fl417-130.00", "d657-290.00", "rat783-460.00", "vm1084-840.00"]
# TIMEOUTS = [140, 220, 520, 520, 1010, 2020, 3180, 4170, 6570, 7830, 10840]

PROBLEMS = ["fl417-160.6"]
TIMEOUTS = [4170]


SEED_FIRST = 3
SEED_LAST = 10


for seed in range(SEED_FIRST, SEED_LAST + 1):
    for problem, timeout in zip(PROBLEMS, TIMEOUTS):
        problem_path = DATASET_DIR + problem + ".json"
        log_path = LOG_DIR + problem + "_" + str(timeout) + "s/"
        if not os.path.exists(log_path):
            os.makedirs(log_path)
        log = log_path + str(seed) + "-" + problem + ".out"
        run = subprocess.run([CMD, "-d", problem_path, "-t", str(timeout), "-o", log, "-c", CONFIG, "-s", str(seed)])
        time.sleep(5)
