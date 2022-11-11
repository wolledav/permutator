#!/usr/bin/python
import copy
import random
import numpy as np
from tools.tspsd.tspsd_common import *


NUM_VERTICES = 90
PROBLEMS_PER_DEGREE = 50
MIN_AVG_DEGREE = 6
MAX_AVG_DEGREE = 20

VD_STEP = (MAX_AVG_DEGREE - MIN_AVG_DEGREE) / 50 # vertex degree sampling step
X_MAX = 100
Y_MAX = 100
EPS = 0.1
ER_STEP = int(NUM_VERTICES/10) # edges to remove at once
OUTDIR = "./data/tspsd/random" + str(NUM_VERTICES) + "_ov_" + str(PROBLEMS_PER_DEGREE) + "/"

# Create dataset directory
if not os.path.exists(OUTDIR):
    os.makedirs(OUTDIR)

# Create list of all vertex->edge combinations
all_vertices, all_edges, all_deletes = get_vertices_edges_deletes(NUM_VERTICES)

# Create PROBLEMS_PER_DEGREE instances for each avg degree
for id in range(PROBLEMS_PER_DEGREE):
    all_degrees = list(np.arange(MIN_AVG_DEGREE - VD_STEP, MAX_AVG_DEGREE + VD_STEP, VD_STEP))
    random.shuffle(all_deletes)
    all_deletes_ = copy.deepcopy(all_deletes)
    edge_removed_cnt = {}
    coords = {}
    delete = {}
    groups = [0] * NUM_VERTICES
    for e in all_edges:
        edge_removed_cnt[e] = 0
    for vertex in all_vertices:
        coords[str(vertex)] = [random.randint(0, X_MAX), random.randint(0, Y_MAX)]
        delete[str(vertex)] = []
    total_removed_cnt = 0
    goal_degree = all_degrees.pop()
    print(all_degrees)
    while goal_degree >= MIN_AVG_DEGREE:
        current_degree = get_exp_degree(edge_removed_cnt, NUM_VERTICES, NUM_VERTICES/2)
        # print(current_degree)
        if abs(current_degree - goal_degree) < EPS: # export instance
            print("goal_degree = " + str(goal_degree) + ", current_degree = " + str(current_degree))
            # EXPORT ------------------------------------------
            data = {}
            data["NAME"] = "random-" + str(NUM_VERTICES) + "-" + "{:.2f}".format(goal_degree) + "-" + str(id)
            data["TYPE"] = "TSPSD"
            data["COMMENT"] = str(NUM_VERTICES) + " random locations, " + str(goal_degree) + " goal degree after n/2 deletes"
            data["EDGE_WEIGHT_TYPE"] = "EUC_2D"
            data["DIMENSION"] = NUM_VERTICES
            data["GROUPS"] = groups
            data["GROUPS_SUM"] = total_removed_cnt
            data["EXP_DEGREE_HALF"] = current_degree
            data["EXP_DEGREES"] = []
            data["EXP_REMOVED_EDGES"] = []
            for steps in range(NUM_VERTICES + 1):
                data["EXP_DEGREES"].append(get_exp_degree(edge_removed_cnt, NUM_VERTICES, steps))
                data["EXP_REMOVED_EDGES"].append(get_exp_sum(edge_removed_cnt, NUM_VERTICES, steps))
            data["NODE_COORDS"] = coords
            data["DELETE"] = delete
            json_data = json.dumps(data, indent=4)
            output_path = OUTDIR + data["NAME"] + ".json"
            with open(output_path, "w") as outfile:
                outfile.write(json_data)
                print("Generated " + output_path)
            goal_degree = all_degrees.pop()
        # EXPORT ------------------------------------------
        else: # delete ER_STEP more edges
            for i in range(ER_STEP):
                if len(all_deletes_) > 0:
                    e = all_deletes_.pop()
                    edge_removed_cnt[e[1]] += 1
                    total_removed_cnt += 1
                    groups[e[0] - 1] += 1
                    delete[str(e[0])].append([str(e[1][0]), str(e[1][1])])
