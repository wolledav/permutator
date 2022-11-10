#!/usr/bin/python
import os
import matplotlib.pyplot as plt
from tools.tspsd.tspsd_common import *
import pandas as pd


LOG_DIRS = [
    "./log/scp-meta/tsp/random24_tsp_100_TSP_3s/", "./log/scp-meta/dense_sampling/random24_ov_100_SCP_3s/", "./log/wcp-meta/dense_sampling/random24_ov_100_WCP_3s/"
]
PROBLEM_DIRS = [
    "./data/tspsd/tsp/random24_tsp_100/", "./data/tspsd/dense_sampling/random24_ov_100/", "./data/tspsd/dense_sampling/random24_ov_100/"
]
LEGENDS = [
    "TSP", "SCP", "WCP"
]
WINDOW_SIZES = [ # must be even
    18, 18, 8
]
PROBLEM_SIZES = [
    24, 24, 24
]

FIG_DIR = "./figures/scp/hamiltonicity/"
TITLE = "24-vertex graph"
DECIMALS = 1
REL_STEP = 0.5

fig_output = FIG_DIR
for log_dir, problem_dir, window_size, problem_size, legend in zip(LOG_DIRS, PROBLEM_DIRS, WINDOW_SIZES, PROBLEM_SIZES, LEGENDS):
    step = int(REL_STEP * problem_size)
    if step == problem_size/2:
        degrees, probs = get_probs_in_half(log_dir)
    else:
        degrees, probs = get_probs_in_step(problem_dir, log_dir, step, DECIMALS)
    dataset = log_dir.split('/')[-2]
    fig_output = fig_output + dataset + ","
    color = plt.plot(degrees, probs, alpha=1, marker='.', linestyle='none', markersize='2')[0].get_color()

    # Filter probabilities by moving average filter
    probs_series = pd.Series(probs)
    windows = probs_series.rolling(window_size)
    probs_smooth = windows.mean().tolist()[window_size - 1:]
    degrees_reduced = degrees[int(window_size/2 - 1):-int(window_size/2)]
    plt.plot(degrees_reduced, probs_smooth, color, label=legend, alpha=0.75)

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

fig_output = fig_output[:-1] + "_STEP=" + str(REL_STEP) + "n.pdf"

plt.legend()
plt.grid()
plt.xlabel("Expected average vertex degree after " + str(REL_STEP) + "n steps")
plt.ylabel("Hamiltonian - probability []")
plt.title(TITLE)

if not os.path.exists(FIG_DIR):
    os.makedirs(FIG_DIR)
plt.savefig(fig_output, bbox_inches='tight')
print("Exported " + fig_output)

plt.show()
