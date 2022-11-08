#!/usr/bin/python

import os
from sortedcontainers import SortedSet
import subprocess
import shutil
import json
import time

time.sleep(21600)

CMD = "./cmake-build-release/scp_meta"
TEST_TIMEOUT = 10
DATASET_DIR = "./data/tspsd/"
LOG_DIR = "./log/scp-meta/"
MAX_INFEASIBLE = 5
DATASETS = ["random100_ov_100"
            ]

for D_NAME in DATASETS:
    problems_cnt = int(D_NAME.split('_')[-1])

    D_PATH = DATASET_DIR + D_NAME + "/"
    LOG_PATH = LOG_DIR + D_NAME + "_" + str(TEST_TIMEOUT) + "s/"
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
