#!/usr/bin/python

import random
import json
import os

NUM_VERTICES = 24
PROBLEMS_PER_SIZE = 2
X_MAX = 100
Y_MAX = 100
OUTDIR = "./data/tspsd/random_tsp24_v2/"


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
#for removed_edges_cnt in [3]:
    for problems_cnt in range(PROBLEMS_PER_SIZE): # generate j problems with i edges removed
        data = {}
        data["NAME"] = "random_tsp-" + str(NUM_VERTICES) + "-" + str(removed_edges_cnt) + "-" + str(problems_cnt)
        data["TYPE"] = "TSPSD"
        data["COMMENT"] = str(NUM_VERTICES) + " random locations, " + str(removed_edges_cnt) + " identical edges deleted for each location"
        data["DIMENSION"] = NUM_VERTICES
        data["EDGE_WEIGHT_TYPE"] = "EUC_2D"
        coords = {}
        delete = {}
        random.shuffle(all_edges)
        for vertex in all_vertices:
            coords[str(vertex)] = [random.randint(0, X_MAX), random.randint(0, Y_MAX)]
            delete[str(vertex)] = all_edges[0:removed_edges_cnt]
        data["NODE_COORDS"] = coords
        data["DELETE"] = delete
        json_data = json.dumps(data, indent=4)
        output_path = OUTDIR + data["NAME"] + ".json"
        with open(output_path, "w") as outfile:
            outfile.write(json_data)
            print("Generated " + output_path)

