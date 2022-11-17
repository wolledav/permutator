#!/usr/bin/python

import random
import copy
import numpy as np
import sys
from tspsd_common import *


TSP_INSTANCE = "./data/tsp/tsplib_10/vm1084.json"
OUTDIR = "./data/tspsd/tsplib_10/vm1084/"

MIN_AVG_DEGREE = 100
MAX_AVG_DEGREE = 1080
VD_STEP = 20
EPS = 0.5
random.seed(1)

data = get_data(TSP_INSTANCE)
name = data['NAME']
data['DELETE'] = {}
NUM_VERTICES = data['DIMENSION']
for node in data['NODE_COORDS']:
    data['DELETE'][node] = []
ER_STEP = int(NUM_VERTICES) # edges to remove at once

# Create dataset directory
if not os.path.exists(OUTDIR):
    os.makedirs(OUTDIR)

vertices, edges = get_vertices_edges(NUM_VERTICES)

# Create PROBLEMS_PER_DEGREE instances for each avg degree
all_degrees = list(np.arange(MIN_AVG_DEGREE - VD_STEP, MAX_AVG_DEGREE + VD_STEP, VD_STEP))
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
        data["NAME"] = name + "-" + str(NUM_VERTICES) + "-" + "{:.2f}".format(goal_degree) + "-" + str(0)
        data["TYPE"] = "TSPSD"
        data["COMMENT"] = str(NUM_VERTICES) + " random locations, " + str(goal_degree) + " goal degree after n/2 deletes"
        data["GROUPS"] = groups
        data["GROUPS_SUM"] = total_removed_cnt
        data["EXP_DEGREE_HALF"] = current_degree
        data["EXP_DEGREES"] = []
        data["EXP_REMOVED_EDGES"] = []
        # for steps in range(NUM_VERTICES + 1):
        #     exp_sum = get_exp_sum(edge_removed_cnt, NUM_VERTICES, steps)
        #     data["EXP_REMOVED_EDGES"].append(exp_sum)
        #     data["EXP_DEGREES"].append((NUM_VERTICES - 1) - (2 * exp_sum)/NUM_VERTICES)
        data["DELETE"] = delete
        json_data = json.dumps(data, indent=4)
        output_path = OUTDIR + data["NAME"] + ".json"
        with open(output_path, "w") as outfile:
            outfile.write(json_data)
            print("Generated " + output_path)
        # EXPORT ------------------------------------------
        goal_degree = all_degrees.pop()
    else: # delete ER_STEP more edges
        i = 0
        while i < ER_STEP:
            vertex = random.choice(vertices)
            edge = random.choice(edges)
            if [str(edge[0]), str(edge[1])] not in delete[str(vertex)]:
                edge_removed_cnt[edge] += 1
                total_removed_cnt += 1
                groups[vertex - 1] += 1
                delete[str(vertex)].append([str(edge[0]), str(edge[1])])
                i += 1
