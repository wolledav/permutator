#!/usr/bin/python

import os
from sortedcontainers import SortedSet
import subprocess
import json

CMD = "./cmake-build-release/scp_meta_stable"
PROBLEM_TYPE = "SCP"
DATASET_DIR = "./data/tspsd/range_estimate/"
LOG_DIR = "./log/scp-meta/"
MAX_INFEASIBLE = 5
DATASETS = ["random10_ov_1", "random20_ov_1", "random30_ov_1", "random40_ov_1", "random50_ov_1", "random60_ov_1", "random70_ov_1", "random80_ov_1", "random90_ov_1", "random100_ov_1", "random200_ov_1", "random400_ov_1"]
TEST_TIMEOUTS = [3, 3, 3, 4, 5, 6, 7, 8, 9, 10, 20, 40]

for D_NAME, TEST_TIMEOUT in zip(DATASETS, TEST_TIMEOUTS):
    problems_cnt = int(D_NAME.split('_')[-1])

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
        print("Solving problems with id " + str(id))
        infeasible_cnt = 0
        prev_log = ""
        for vd in reversed(vds):
            problem = prefix + '{0:.2f}'.format(vd) + "-" + str(id)
            log = LOG_PATH + problem + ".out"
            if infeasible_cnt < MAX_INFEASIBLE: # Solve
                run = subprocess.run([CMD, "-d", D_PATH + problem + ".json", "-t", str(TEST_TIMEOUT), "-o", log])
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
