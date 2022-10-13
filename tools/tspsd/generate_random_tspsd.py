#!/usr/bin/python

import random
import json
import os


def positiveIntegersWithSum(n, total):
    ls = [0]
    ret = []
    while len(ls) < n:
        c = random.randint(1, total - 1)
        found = False
        for j in range(1, len(ls)):
            if ls[j] == c:
                found = True
                break
        if not found:
            ls.append(c)
    ls.sort()
    ls.append(total)
    for i in range(1, len(ls)):
        ret.append(ls[i] - ls[i - 1])
    return ret


def integersWithSum(n, total):
    ret = positiveIntegersWithSum(n, total + n)
    for i in range(0, len(ret)):
        ret[i] = ret[i] - 1
    return ret

NUM_VERTICES = 24
PROBLEMS_PER_SIZE = 2
X_MAX = 100
Y_MAX = 100
OUTDIR = "./data/tspsd/random24_v2/"

if not os.path.exists(OUTDIR):
    os.makedirs(OUTDIR)

max_edges = int(NUM_VERTICES * (NUM_VERTICES - 1) / 2)
all_vertices = list(range(1, NUM_VERTICES + 1))
all_edges = []
for node1 in all_vertices:
    for node2 in all_vertices:
        if node1 < node2:
            all_edges.append([str(node1), str(node2)])

for removed_edges_cnt in range(max_edges + 1):
    for problems_cnt in range(PROBLEMS_PER_SIZE):  # generate j problems with i edges removed
        data = {}
        data["NAME"] = "random-" + str(NUM_VERTICES) + "-" + str(removed_edges_cnt / 2) + "-" + str(problems_cnt)
        data["TYPE"] = "TSPSD"
        data["COMMENT"] = str(NUM_VERTICES) + " random locations, " + str(
            removed_edges_cnt / 2) + " edges deleted on average"
        data["DIMENSION"] = NUM_VERTICES
        data["EDGE_WEIGHT_TYPE"] = "EUC_2D"
        coords = {}
        delete = {}
        random.shuffle(all_edges)
        shuffled_edges = all_edges.copy()
        groups = integersWithSum(NUM_VERTICES, removed_edges_cnt)
        for vertex in all_vertices:
            coords[str(vertex)] = [random.randint(0, X_MAX), random.randint(0, Y_MAX)]  # fill coords
            edges_cnt = groups.pop(0)   # fill delete function
            delete[str(vertex)] = []
            for i in range(edges_cnt):
                delete[str(vertex)].append(shuffled_edges.pop(0))
        data["NODE_COORDS"] = coords
        data["DELETE"] = delete

        json_data = json.dumps(data, indent=4)
        output_path = OUTDIR + data["NAME"] + ".json"
        with open(output_path, "w") as outfile:
            outfile.write(json_data)
            print("Generated " + output_path)
