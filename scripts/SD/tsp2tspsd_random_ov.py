#!/usr/bin/python

import random
import copy
import numpy as np
import sys
from tspsd_common import *


TSP_INSTANCE = "./data/tsp/tsplib_10/rat783.json"
OUTDIR = "./data/tspsd/tsplib_10/AVD/"

VD_STEP = 0.1
MIN_AVG_DEGREE = 460
MAX_AVG_DEGREE = MIN_AVG_DEGREE



EPS = 0.25
SEED = 1
random.seed(SEED)

data = get_data(TSP_INSTANCE)
name = data['NAME']
data['DELETE'] = {}
NUM_VERTICES = data['DIMENSION']
for node in data['NODE_COORDS']:
    data['DELETE'][node] = []

ER_STEP = int(NUM_VERTICES/2) # edges to remove at once


# Create dataset directory
if not os.path.exists(OUTDIR):
    os.makedirs(OUTDIR)

vertices, edges, deletes = get_vertices_edges_deletes(NUM_VERTICES)



# Create PROBLEMS_PER_DEGREE instances for each avg degree
all_degrees = list(np.arange(MIN_AVG_DEGREE - VD_STEP, MAX_AVG_DEGREE + VD_STEP, VD_STEP))
random.shuffle(deletes)
edge_removed_cnt = {}
delete = {}
groups = [0] * NUM_VERTICES
for e in edges:
    edge_removed_cnt[e] = 0
for vertex in vertices:
    delete[str(vertex)] = []
total_removed_cnt = 0
goal_degree = all_degrees.pop()
print(all_degrees)
while goal_degree >= MIN_AVG_DEGREE:
    current_degree = get_exp_degree(edge_removed_cnt, NUM_VERTICES, NUM_VERTICES/2)
    print(current_degree)
    if abs(current_degree - goal_degree) < EPS: # export instance
        print("goal_degree = " + str(goal_degree) + ", current_degree = " + str(current_degree))
        # EXPORT ------------------------------------------
        data["NAME"] = name + "-" + "{:.2f}".format(goal_degree) + "-" + str(SEED)
        data["TYPE"] = "TSPSD"
        data["COMMENT"] = str(NUM_VERTICES) + " random locations, " + str(goal_degree) + " goal degree after n/2 deletes"
        data["GROUPS"] = groups
        data["GROUPS_SUM"] = total_removed_cnt
        data["EXP_DEGREE_HALF"] = current_degree
        data["EXP_DEGREES"] = []
        data["EXP_REMOVED_EDGES"] = []
        for steps in range(1, NUM_VERTICES + 1):
            exp_sum = get_exp_sum(edge_removed_cnt, NUM_VERTICES, steps)
            data["EXP_REMOVED_EDGES"].append(exp_sum)
            data["EXP_DEGREES"].append((NUM_VERTICES - 1) - (2 * exp_sum)/NUM_VERTICES)
        data["AVD"] = np.mean(data["EXP_DEGREES"])
        data["DELETE"] = delete
        json_data = json.dumps(data, indent=4)
        output_path = OUTDIR + data["NAME"] + ".json"
        with open(output_path, "w") as outfile:
            outfile.write(json_data)
            print("Generated " + output_path)
        # EXPORT ------------------------------------------
        goal_degree = all_degrees.pop()
    else: # delete ER_STEP more edges
        for i in range(ER_STEP):
            if len(deletes) > 0:
                e = deletes.pop()
                edge_removed_cnt[e[1]] += 1
                total_removed_cnt += 1
                groups[e[0] - 1] += 1
                delete[str(e[0])].append([str(e[1][0]), str(e[1][1])])
