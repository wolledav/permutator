#!/usr/bin/python
import os
import matplotlib.pyplot as plt
from tools.tspsd.tspsd_common import get_probs_in_half
import pandas as pd


INPUT_DIRS = [
    # "./log/scp-meta/random24_ov_100_SCP_3s/",
    # "./log/scp-meta/random50_ov_100_SCP_5s/",
    # "./log/scp-meta/random100_ov_100_SCP_10s/"

    "./log/scp-meta/random24_ov_100_SCP_3s/",
    "./log/scp-meta/random24_tsp_100_TSP_3s/",
    "./log/wcp-meta/random24_ov_100_WCP_3s/"

    # "./log/scp-meta/random24_ov_100_SCP_3s/",
    # "./log/scp-meta/random24_tsp_100_TSP_3s/",
    # "./log/scp-meta/random50_ov_100_SCP_5s/",
    # "./log/scp-meta/random50_tsp_100_TSP_5s/"
]

WINDOW_SIZES = [ # must be even
    # 24,
    # 24,
    # 6

    24,
    24,
    24

    # 24,
    # 24,
    # 24,
    # 24
]

for input_dir, window_size in zip(INPUT_DIRS, WINDOW_SIZES):
    degrees, probs = get_probs_in_half(input_dir)
    dataset = input_dir.split('/')[-2]
    color = plt.plot(degrees, probs, alpha=0.5)[0].get_color()

    # Filter probabilities by moving average filter
    probs_series = pd.Series(probs)
    windows = probs_series.rolling(window_size)
    probs_smooth = windows.mean().tolist()[window_size - 1:]
    degrees_reduced = degrees[int(window_size/2 - 1):-int(window_size/2)]
    plt.plot(degrees_reduced, probs_smooth, color, label=dataset)

    # Estimate degree of p = 0.5
    prob_lb = 0
    prob_ub = 1
    lb, ub = -1, -1
    prob_target = 0.5
    for deg, prob in zip(degrees_reduced, probs_smooth):
        if prob_target >= prob > prob_lb:
            prob_lb = prob
            lb = deg
        elif prob_target <= prob < prob_ub:
            prob_ub = prob
            ub = deg
    target = lb + (prob_target - prob_lb) * (ub - lb)/(prob_ub - prob_lb)
    plt.scatter(target, prob_target, marker='x', color=color)
    size = int(dataset.split('_')[0].split('random')[1])
    print(dataset, size, target)

plt.legend()
plt.grid()
plt.xlabel("Expected average vertex degree after n/2 steps")
plt.ylabel("Hamiltonian - probability []")
plt.show()
