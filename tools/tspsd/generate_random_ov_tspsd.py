#!/usr/bin/python

import random
import json
import os
import math
import numpy as np


def get_p_e_removed(n, k, steps):
    if k == 0:
        return 0
    elif k > n - int(steps):
        return 1
    else:
        return (math.comb(n, k) - math.comb(n - int(steps), k)) / math.comb(n, k)


def get_exp_sum(edge_rem_cnt, n, steps):
    sum = 0
    for e in edge_rem_cnt:
        k = edge_rem_cnt[e]
        sum += get_p_e_removed(n, k, steps)
    return sum


def get_p_e_removed_v2(n, k, steps):
    prod = 1
    for i in range(k):
        prod *= ((n - steps) - i)/(n - i)
    return 1 - prod


def get_exp_degree(edge_rem_cnt, n, steps):
    sum = get_exp_sum(edge_rem_cnt, n, steps)
    return (n - 1) - (2 * sum)/n


NUM_VERTICES = 24
PROBLEMS_PER_DEGREE = 1
MIN_AVG_DEGREE = 0
MAX_AVG_DEGREE = NUM_VERTICES - 1
STEP = 0.5
X_MAX = 100
Y_MAX = 100
EPS = 0.025
OUTDIR = "./data/tspsd/random" + str(NUM_VERTICES) + "_ov_" + str(PROBLEMS_PER_DEGREE) + "/"

# Create dataset directory
if not os.path.exists(OUTDIR):
    os.makedirs(OUTDIR)

# Create list of all vertex->edge combinations
all_vertices = list(range(1, NUM_VERTICES + 1))
all_edges = []
all_deletes = []
for e_from in all_vertices:
    for e_to in all_vertices:
        if e_from < e_to:
            all_edges.append(tuple([e_from, e_to]))
for v in all_vertices:
    for e in all_edges:
        all_deletes.append([v, e])

# Create PROBLEMS_PER_DEGREE instances for each avg degree
for id in range(PROBLEMS_PER_DEGREE):
    all_degrees = list(np.arange(MIN_AVG_DEGREE - STEP, MAX_AVG_DEGREE + STEP, STEP))
    random.shuffle(all_deletes)
    all_deletes_ = all_deletes.copy()
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
    while goal_degree >= MIN_AVG_DEGREE:
        current_degree = get_exp_degree(edge_removed_cnt, NUM_VERTICES, NUM_VERTICES/2)
        if abs(current_degree - goal_degree) < EPS: # export instance
            print("goal_degree = " + str(goal_degree) + ", current_degree = " + str(current_degree))
            # EXPORT ------------------------------------------
            data = {}
            data["NAME"] = "random-" + str(NUM_VERTICES) + "-" + str(goal_degree) + "-" + str(id)
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
            # EXPORT ------------------------------------------
            goal_degree = all_degrees.pop()
        else: # delete one more edge
            e = all_deletes_.pop()
            edge_removed_cnt[e[1]] += 1
            total_removed_cnt += 1
            groups[e[0] - 1] += 1
            delete[str(e[0])].append([str(e[1][0]), str(e[1][1])])
