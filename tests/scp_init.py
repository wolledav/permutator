#!/usr/bin/python

import os
import subprocess
import time

CMD = "./cmake-build-release/scp_meta_stable"
CONFIG = "./configs/SCP_config_opt_random.json"
PROBLEM_TYPE = "SCP"

DATASET_DIR = "./data/tspsd/tsplib_10/selected/"
LOG_DIR = "./log/scp-meta/tsplib_10/selected_init/"
INIT_DIR = "./log/scp-exact/init_solutions/"
PROBLEMS = ["berlin52-5.00"]
TIMEOUTS = [520]
CNT = 55

for problem, timeout in zip(PROBLEMS, TIMEOUTS):
    problem_path = DATASET_DIR + problem + ".json"
    log_path = LOG_DIR + problem + "_" + str(timeout) + "s/"
    if not os.path.exists(log_path):
        os.makedirs(log_path)
    for seed in range(CNT):
        log = log_path + str(seed) + "-" + problem + ".out"
        run = subprocess.run([CMD, "-d", problem_path, "-t", str(timeout), "-o", log, "-c", CONFIG, "-s", str(seed), "-i", INIT_DIR + problem + ".out"])
        time.sleep(5)
