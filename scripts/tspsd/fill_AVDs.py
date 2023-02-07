#!/usr/bin/python

import os
import numpy as np
import json
from tspsd_common import *


# TODO works only with exact solutions
DATA_DIR = "./data/tspsd/ham_bound_v1/random150_ov_50_v2/"
LOG_DIR = "./log/TSPSD-exact/ham_bound_v1/random150_ov_50_v2/"
FIRST_EAVD = 1

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
