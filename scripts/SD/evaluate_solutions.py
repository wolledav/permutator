#!/usr/bin/python

import os
import subprocess
import time

PROBLEM_DIR = "./data/tspsd/tsplib_10/selected/"
LOG_DIR = "./log/TSPSD-exact/selected_12h/"
# LOG_DIR = "./log/TSPSD-exact/first_valid/"
CMD = "./cmake-build-release/TSPSD_meta"


problems = os.listdir(PROBLEM_DIR)
for problem in problems:
    problem_path = PROBLEM_DIR + problem
    log_path = LOG_DIR + problem

    print(problem_path)
    print(log_path)
    subprocess.run([CMD, "-d", problem_path, "-i", log_path, "-t", str(5)])
    time.sleep(1)
    print()
