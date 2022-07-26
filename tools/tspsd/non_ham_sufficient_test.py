#!/usr/bin/python
import os
import json
import matplotlib.pyplot as plt
import itertools


def is_reachable(node, nodes, subset, deleted):
    reachable = False
    for n in nodes:
        if not deleted[n][node] and n not in subset:
            reachable = True
    return reachable


def test_instance(path, k):
    with open(path) as f:
        data = json.load(f)
        size = data["DIMENSION"]
        nodes = {*range(1, size + 1)}
        subsets = [*itertools.combinations(nodes, k)]
        reachable_subset = False
        for subset in subsets:
            # Init delete matrix
            deleted = [[False for i in range(size + 1)] for j in range(size + 1)]
            for node in nodes: # process everything but subset
                if node not in subset:
                    for e in data["DELETE"][str(node)]:
                        deleted[int(e[0])][int(e[1])] = True
                        deleted[int(e[1])][int(e[0])] = True
            first = subset[0]
            last = subset[-1]
            if not deleted[first][last] and is_reachable(first, nodes, subset, deleted) and is_reachable(last, nodes, subset, deleted):
                # print("Reachable subset:", subset)
                reachable_subset = True
    return reachable_subset


PROBLEM_DIR = "./data/tspsd/random24_ov_100/"
SOLUTION_DIR = "./log/scp-meta/random24_ov_100_SCP_3s/"
k = 1

total = 0
infeasible = 0
solved = 0
problems = os.listdir(PROBLEM_DIR)
for problem in problems:
    problem_path = PROBLEM_DIR + problem
    if not test_instance(problem_path, k):
        infeasible += 1
    total += 1

    solution_path = SOLUTION_DIR + problem.split('.json')[-2] + ".out"
    with open(solution_path) as f:
        data = json.load(f)
        if data["solution"]["is_feasible"]:
            solved += 1

    print(problem, infeasible, solved, total)


