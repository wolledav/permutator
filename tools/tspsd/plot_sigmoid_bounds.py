#!/usr/bin/python
import matplotlib.pyplot as plt
from tools.tspsd.tspsd_common import *
import pandas as pd


LOG_DIRS = [
    # "./log/scp-meta/tsp/random24_tsp_100_TSP_3s/", "./log/scp-meta/dense_sampling/random24_ov_100_SCP_3s/", "./log/wcp-meta/dense_sampling/random24_ov_100_WCP_3s/"
    "./log/scp-meta/ham_bound_v1/random10_ov_50_SCP_10s/", "./log/scp-meta/ham_bound_v1/random20_ov_50_SCP_20s/", "./log/scp-meta/ham_bound_v1/random30_ov_50_SCP_30s/", "./log/scp-meta/ham_bound_v1/random40_ov_50_SCP_40s/", "./log/scp-meta/ham_bound_v1/random50_ov_50_SCP_50s/"
]
PROBLEM_DIRS = [
    # "./data/tspsd/tsp/random24_tsp_100/", "./data/tspsd/dense_sampling/random24_ov_100/", "./data/tspsd/dense_sampling/random24_ov_100/"
    "./data/tspsd/random10_ov_50/", "./data/tspsd/random20_ov_50/", "./data/tspsd/random30_ov_50/", "./data/tspsd/random40_ov_50/", "./data/tspsd/random50_ov_50/"
]
LEGENDS = [
    # "TSP", "SCP", "WCP"
    "random10", "random20", "random30", "random40", "random50"
]
WINDOW_SIZES = [ # must be even
    # 18, 18, 8
    10, 10, 10, 10, 10
]
# TITLE = "24-vertex graph"
TITLE = "SCP_ham_bound_v1"

FIG_DIR = "./figures/scp/hamiltonicity/"
DECIMALS = 1
REL_STEP = 0.75
BOUNDS_FILE = "./log/scp-meta/ham_bound_v1/bounds_" + str(REL_STEP) + "n.json"
PROBLEM_TYPE = "SCP"

fig_output = FIG_DIR
bound_vd = []
problem_sizes = []
for log_dir, problem_dir, window_size, legend in zip(LOG_DIRS, PROBLEM_DIRS, WINDOW_SIZES, LEGENDS):
    dataset = log_dir.split('/')[-2]
    problem_size = int(dataset.split('_')[0].split('random')[1])
    fig_output = fig_output + dataset + ","
    step = int(REL_STEP * problem_size)
    if step == problem_size/2:
        degrees, probs = get_probs_in_half(log_dir)
    else:
        degrees, probs = get_probs_in_step(problem_dir, log_dir, step, DECIMALS)
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
    bound_vd.append(target)
    problem_sizes.append(problem_size)


# Export vertex degrees of prob. bound
bounds_dict = {}
bounds_dict["problem_type"] = PROBLEM_TYPE
bounds_dict["log_dirs"] = LOG_DIRS
bounds_dict["problem_dirs"] = PROBLEM_DIRS
bounds_dict["sizes"] = problem_sizes
bounds_dict["bounds"] = bound_vd
bounds_dict["rel_step"] = REL_STEP
json_object = json.dumps(bounds_dict, indent=4)
with open(BOUNDS_FILE, "w") as outfile:
    outfile.write(json_object)


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

