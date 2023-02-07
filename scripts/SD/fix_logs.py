#!/usr/bin/python
import os
import json

LOG_DIR = "./log/TSPSD-exact/ham_bound_v1/random100_ov_50/"
MAX_DEPTH = 100

logs = os.listdir(LOG_DIR)
for log in logs:
    path = LOG_DIR + log
    with open(path, 'r') as file:
        data = json.load(file)
        depth = data["solution"]["max_depth"]
        data["solution"]["is_feasible"] = depth == MAX_DEPTH
    with open(path, 'w') as file:
        json.dump(data, file)
    print(log, depth == MAX_DEPTH)
