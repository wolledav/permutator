#!/usr/bin/python
import os
from tspsd_common import *

# TODO plot infeasibility detection ratio in different depths

LOG_DIR = "./log/scp-exact/ham_bound_v1/random20_ov_50/"

files = os.listdir(LOG_DIR)
for file in files:
    data = get_data(LOG_DIR + file)
    name = data["name"]
    feasible = data["solution"]["is_feasible"]
    depth = data["solution"]["max_depth"]
    EAVD = float(name.split('-')[2])
    print(EAVD, feasible, depth)
