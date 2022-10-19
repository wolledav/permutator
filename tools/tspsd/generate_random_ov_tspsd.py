#!/usr/bin/python

import random
import json
import os
import math


def boundedIntegersWithSum(n, total, bound):
    if n * bound < total:
        print("Invalid input parameters!")
        return
    arr = [0] * n
    added = 0
    while added < total:
        i = random.randint(0, n - 1)
        if arr[i] < bound:
            arr[i] += 1
            added += 1
    return arr


NUM_VERTICES = 24
PROBLEMS_PER_SIZE = 1
X_MAX = 100
Y_MAX = 100
OUTDIR = "./data/tspsd/random" + str(NUM_VERTICES) + "_ov_" + str(PROBLEMS_PER_SIZE) + "/"
MIN_REMOVED_EDGES = 0

if not os.path.exists(OUTDIR):
    os.makedirs(OUTDIR)

num_edges = int(NUM_VERTICES * (NUM_VERTICES - 1) / 2)
max_removed_edges = num_edges * NUM_VERTICES
all_vertices = list(range(1, NUM_VERTICES + 1))
all_edges = []
for node1 in all_vertices:
    for node2 in all_vertices:
        if node1 < node2:
            all_edges.append([str(node1), str(node2)])

print("|V| = " + str(NUM_VERTICES))
print("|E| = " + str(num_edges))
print("max_removed_edges = " + str(max_removed_edges))

for removed_edges_cnt in range(MIN_REMOVED_EDGES, max_removed_edges + 1):
# for removed_edges_cnt in [10]:
    print("Generating instances with " + str(removed_edges_cnt) + " removed edges")

    for problems_cnt in range(PROBLEMS_PER_SIZE):  # generate problems_cnt problems with removed_edges_cnt edges removed
        groups_ = boundedIntegersWithSum(NUM_VERTICES, removed_edges_cnt, num_edges)
        groups = groups_.copy()
        coords = {}
        delete = {}
        edge_removed_cnt = {}
        for edge in all_edges:
            edge_removed_cnt[tuple(edge)] = 0

        for vertex in all_vertices:
            coords[str(vertex)] = [random.randint(0, X_MAX), random.randint(0, Y_MAX)]  # fill coords
            edges_cnt = groups_.pop(0)   # fill delete function
            random.shuffle(all_edges)
            shuffled_edges = all_edges.copy()
            delete[str(vertex)] = []
            for i in range(edges_cnt):
                edge = shuffled_edges.pop(0)
                delete[str(vertex)].append(edge)
                edge_removed_cnt[(tuple(edge))] += 1

        mean_removed_cnt = 0
        distinct_removed_cnt = 0
        for edge in edge_removed_cnt:
            n = NUM_VERTICES
            k = edge_removed_cnt[edge]
            inc = 0
            if k == 0:
                inc = 0
            elif k > math.floor(n/2):
                inc = 1
                distinct_removed_cnt += 1
            else:
                inc = (math.comb(n, k) - math.comb(math.floor(n/2), k)) / math.comb(n, k)
                distinct_removed_cnt += 1
            mean_removed_cnt += inc

        data = {}
        data["NAME"] = "random-" + str(NUM_VERTICES) + "-" + str(removed_edges_cnt) + "-" + str(problems_cnt)
        data["TYPE"] = "TSPSD"
        data["COMMENT"] = str(NUM_VERTICES) + " random locations, " + str(removed_edges_cnt) + " edges deleted in total"
        data["GROUPS"] = groups
        data["DISTINCT_REMOVED_HALF"] = mean_removed_cnt
        data["DISTINCT_REMOVED"] = distinct_removed_cnt
        data["TOTAL_REMOVED"] = removed_edges_cnt
        data["DIMENSION"] = NUM_VERTICES
        data["EDGE_WEIGHT_TYPE"] = "EUC_2D"
        data["NODE_COORDS"] = coords
        data["DELETE"] = delete


        json_data = json.dumps(data, indent=4)
        output_path = OUTDIR + data["NAME"] + ".json"
        with open(output_path, "w") as outfile:
            outfile.write(json_data)
            print("Generated " + output_path)
