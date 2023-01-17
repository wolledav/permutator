#!/usr/bin/python

import os
from sortedcontainers import SortedSet
import subprocess
import json
import time

CMD = "./cmake-build-release/wcp_meta_stable"
PROBLEM_TYPE = "WCP"
DATASET_DIR = "./data/tspsd/ham_bound_v1/"
LOG_DIR = "./log/wcp-meta/ham_bound_v1/"
MAX_INFEASIBLE = 3
# DATASETS = ["random10_ov_50", "random20_ov_50", "random30_ov_50", "random40_ov_50", "random50_ov_50", "random60_ov_50", "random70_ov_50", "random80_ov_50", "random90_ov_50", "random100_ov_50"]
DATASETS = ["random150_ov_50_v2"]
# TEST_TIMEOUTS = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
TEST_TIMEOUTS = [150]
CONFIG = "./configs/SCP_config_random.json"
MIN_ID = 0



for D_NAME, TEST_TIMEOUT in zip(DATASETS, TEST_TIMEOUTS):
    # problems_cnt = int(D_NAME.split('_')[-1])

    D_PATH = DATASET_DIR + D_NAME + "/"
    LOG_PATH = LOG_DIR + D_NAME + "_" + PROBLEM_TYPE + "_" + str(TEST_TIMEOUT) + "s/"
    if not os.path.exists(LOG_PATH):
        os.makedirs(LOG_PATH)

    # Get vertex degrees and ids
    ids = SortedSet()
    vds = SortedSet()
    prefix = ''
    problems = os.listdir(D_PATH)
    for problem in problems:
        mess = problem.split('-')
        id = int(mess[-1].split('.')[0])
        vd = float(mess[-2])
        prefix = mess[0] + '-' + mess[1] + '-'
        ids.add(id)
        vds.add(vd)

    # Solve
    for id in ids:
        if id >= MIN_ID:
            print("Solving problems with id " + str(id))
            infeasible_cnt = 0
            prev_log = ""
            for vd in reversed(vds):
                problem = prefix + '{0:.2f}'.format(vd) + "-" + str(id)
                log = LOG_PATH + problem + ".out"
                if infeasible_cnt < MAX_INFEASIBLE: # Solve
                    run = subprocess.run([CMD, "-d", D_PATH + problem + ".json", "-t", str(TEST_TIMEOUT), "-o", log, "-c", CONFIG])
                    print("Exported " + log)
                    ret = run.returncode
                    if ret == 0:
                        infeasible_cnt = 0
                    else:
                        infeasible_cnt += 1
                else: # Give up and copy prev_log to log
                    f = open(prev_log)
                    data = json.load(f)
                    f.close()
                    data["name"] = problem
                    json_object = json.dumps(data, indent=4)
                    with open(log, "w") as outfile:
                        outfile.write(json_object)
                    print("Copied " + prev_log + " to " + log)
                prev_log = log
                print()
