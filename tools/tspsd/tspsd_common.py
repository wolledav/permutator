#!/usr/bin/python
import os
import json
import math


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
