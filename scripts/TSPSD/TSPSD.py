#!/usr/bin/python

import os
import subprocess
import time

CMD = "./cmake-build-release/TSPSD_meta_stable"
CONFIG = "./configs/TSPSD_config_opt_random.json"
PROBLEM_TYPE = "TSPSD"

DATASET_DIR = "./data/tspsd/tsplib_10/selected/"
LOG_DIR = "./log/TSPSD-meta/tsplib_10/selected/"
PROBLEMS = ["berlin52-10.4"]
TIMEOUTS = [520]
CNT = 60

for problem, timeout in zip(PROBLEMS, TIMEOUTS):
    problem_path = DATASET_DIR + problem + ".json"
    log_path = LOG_DIR + problem + "_" + str(timeout) + "s/"
    if not os.path.exists(log_path):
        os.makedirs(log_path)
    for seed in range(CNT):
        log = log_path + str(seed) + "-" + problem + ".out"
        run = subprocess.run([CMD, "-d", problem_path, "-t", str(timeout), "-o", log, "-c", CONFIG, "-s", str(seed)])
        time.sleep(5)
