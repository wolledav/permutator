#!/usr/bin/python
import os
import json
import math
import itertools
import sys

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


def get_probs_in_half(input_dir):
    """
    :param:
    input_dir: directory of tspsd solution logs
    :return:
    degrees_sorted: sorted list of EAVD in n/2, read from log name;
    prob_list: list of feasibility probabilities for each degree
    """
    files = os.listdir(input_dir)
    deg2cnt = {} # avg_degree -> [feasible, total]
    for filename in sorted(files):
        path = input_dir + filename
        with open(path) as f:
            data = json.load(f)
            name = data["name"]
            feasible = data["solution"]["is_feasible"]
            parsed = name.split('-')
            avg_degree = float(parsed[2])
            if avg_degree not in deg2cnt.keys():
                deg2cnt[avg_degree] = [0, 0]
            if feasible:
                deg2cnt[avg_degree][0] += 1
            deg2cnt[avg_degree][1] += 1
    degrees_sorted = sorted(deg2cnt.keys())
    feasible_list = []
    total_list = []
    prob_list = []
    for degree in degrees_sorted:
        feasible_list.append(deg2cnt[degree][0])
        total_list.append(deg2cnt[degree][1])
        prob_list.append(deg2cnt[degree][0]/deg2cnt[degree][1])
    return degrees_sorted, prob_list


def get_data(json_path):
    with open(json_path) as f:
        data = json.load(f)
        return data


def get_probs_in_step(problem_dir, log_dir, step, decimals):
    solutions = os.listdir(log_dir)
    deg2cnt = {} # avg_degree -> [feasible, total]
    for solution in solutions:
        sol_data = get_data(log_dir + solution)
        feasible = sol_data["solution"]["is_feasible"]
        name = sol_data["name"].split('.json')[0]
        problem_data = get_data(problem_dir + name + ".json")
        exp_degrees = problem_data["EXP_DEGREES"]
        degree = round(exp_degrees[step], decimals)
        if degree not in deg2cnt.keys():
            deg2cnt[degree] = [0, 0]
        if feasible:
            deg2cnt[degree][0] += 1
        deg2cnt[degree][1] += 1
    degrees_sorted = sorted(deg2cnt.keys())
    feasible_list = []
    total_list = []
    prob_list = []
    for degree in degrees_sorted:
        feasible_list.append(deg2cnt[degree][0])
        total_list.append(deg2cnt[degree][1])
        prob = deg2cnt[degree][0]/deg2cnt[degree][1]
        prob_list.append(prob)
    return degrees_sorted, prob_list


def get_probs_in_AVD(problem_dir, log_dir, decimals):
    solutions = os.listdir(log_dir)
    deg2cnt = {} # avg_degree -> [feasible, total]
    for solution in solutions:
        sol_data = get_data(log_dir + solution)
        feasible = sol_data["solution"]["is_feasible"]
        name = sol_data["name"].split('.json')[0]
        problem_data = get_data(problem_dir + name + ".json")
        exp_degrees = problem_data["EXP_DEGREES"]
        degree = round(np.mean(exp_degrees[1:]), decimals)
        if degree not in deg2cnt.keys():
            deg2cnt[degree] = [0, 0]
        if feasible:
            deg2cnt[degree][0] += 1
        deg2cnt[degree][1] += 1
    degrees_sorted = sorted(deg2cnt.keys())
    feasible_list = []
    total_list = []
    prob_list = []
    for degree in degrees_sorted:
        feasible_list.append(deg2cnt[degree][0])
        total_list.append(deg2cnt[degree][1])
        prob = deg2cnt[degree][0]/deg2cnt[degree][1]
        prob_list.append(prob)
    return degrees_sorted, prob_list


def get_vertices_edges_deletes(num_vertices):
    vertices = list(range(1, num_vertices + 1))
    edges = []
    deletes = []
    for e_from, e_to in itertools.product(vertices, vertices):
        if e_from < e_to:
            edges.append(tuple([e_from, e_to]))
    for v, e in itertools.product(vertices, edges):
        deletes.append([v, e])
    return vertices, edges, deletes


def get_vertices_edges(num_vertices):
    vertices = list(range(1, num_vertices + 1))
    edges = []
    for e_from, e_to in itertools.product(vertices, vertices):
        if e_from < e_to:
            edges.append(tuple([e_from, e_to]))
    return vertices, edges


def get_cutoff_success(log_dir, max_depth):
    stats = {}
    files = os.listdir(log_dir)
    for file in files:
        data = get_data(log_dir + file)
        name = data["name"]
        feasible = data["solution"]["is_feasible"]
        depth = data["solution"]["max_depth"]
        # EAVD = float(name.split('-')[2])
        AVD = round(data["AVD"], 1)
        if AVD not in stats.keys():
            stats[AVD] = {}
            stats[AVD]["feasible"] = 0
            stats[AVD]["infeasible"] = 0
            stats[AVD]["total"] = 0
            stats[AVD]["depths"] = []
        if feasible:
            stats[AVD]["feasible"] += 1
        else:
            stats[AVD]["infeasible"] += 1
        stats[AVD]["total"] += 1
        stats[AVD]["depths"].append(depth)
    AVDs = []
    infeasible_detected = []
    infeasible_cnts = []
    total_cnts = []
    for AVD in sorted(stats.keys()):
        AVDs.append(AVD)
        infeasible_cnts.append(stats[AVD]["infeasible"])
        total_cnts.append(stats[AVD]["total"])
        depth_cnt = 0
        for depth in range(max_depth + 1):
            depth_cnt += stats[AVD]["depths"].count(depth)
        infeasible_detected.append(min(depth_cnt, stats[AVD]["infeasible"]))
    return AVDs, infeasible_detected, infeasible_cnts, total_cnts
