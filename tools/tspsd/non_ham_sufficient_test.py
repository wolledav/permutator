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
        result = False
        for subset in subsets:
            # Init delete matrix
            deleted = [[False for i in range(size + 1)] for j in range(size + 1)]
            for node in nodes: # process everything but subset
                if node not in subset:
                    for e in data["DELETE"][str(node)]:
                        deleted[int(e[0])][int(e[1])] = True
                        deleted[int(e[1])][int(e[0])] = True
            # print("subset: ", subset)
            permutations = itertools.permutations(subset)
            for permutation in permutations:
                reachable_perm = True
                reachable_perm = reachable_perm and is_reachable(permutation[0], nodes, subset, deleted)
                reachable_perm = reachable_perm and is_reachable(permutation[-1], nodes, subset, deleted)
                # reachable_perm = reachable_perm and not deleted[permutation[0]][permutation[-1]]
                for i in range(len(permutation) - 1):
                    reachable_perm = reachable_perm and not deleted[permutation[i]][permutation[i + 1]]
                if reachable_perm:
                    # print("Reachable permutation:", permutation)
                    result = True
    return result


PROBLEM_DIR = "./data/tspsd/dense_sampling/random24_ov_100/"
SOLUTION_DIR = "./log/scp-meta/dense_sampling/random24_ov_100_SCP_3s/"
k = 2

total = 0
infeasible = 0
solved = 0
problems = os.listdir(PROBLEM_DIR)
# problems = ["random-24-4.00-70.json"]

for problem in problems:
    problem_path = PROBLEM_DIR + problem
    if not test_instance(problem_path, k):
        print(problem, " infeasible")
        infeasible += 1
    total += 1

    solution_path = SOLUTION_DIR + problem.split('.json')[-2] + ".out"
    with open(solution_path) as f:
        data = json.load(f)
        if data["solution"]["is_feasible"]:
            solved += 1

    # print(problem, infeasible, solved, total)


