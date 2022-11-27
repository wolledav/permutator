#!/usr/bin/python

import os
import numpy as np
import json
from tspsd_common import *

DATA_DIR = "./data/tspsd/dense_sampling/random24_ov_AVD_100/"
LOG_DIR = "./log/scp-exact/dense_sampling/random24_ov_100_AVD_SCP_v2/"
FIRST_EAVD = 0

problems = os.listdir(DATA_DIR)
for problem in problems:
    problem_path = DATA_DIR + problem
    log_path = LOG_DIR + problem
    problem_data = get_data(problem_path)
    AVD = np.mean(problem_data["EXP_DEGREES"][FIRST_EAVD:])
    log_data = get_data(log_path)
    log_data["AVD"] = AVD
    with open(log_path, 'w') as f:
        json.dump(log_data, f)
