#!/usr/bin/python
import os
import json


def get_probs_in_half(input_dir):
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
