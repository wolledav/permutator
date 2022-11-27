#!/usr/bin/python
import matplotlib.pyplot as plt
from tools.tspsd.tspsd_common import *
import pandas as pd

# Hamiltonicity on random24 dataset
LOG_DIRS = [ "./log/scp-exact/dense_sampling/random24_tsp_100_HCP/", "./log/scp-exact/dense_sampling/random24_ov_100_AVD_SCP/", "./log/scp-meta/dense_sampling/random24_ov_AVD_100_SCP_24s/", "./log/wcp-meta/dense_sampling/random24_ov_AVD_100_WCP_24s/"]
PROBLEM_DIRS = ["./data/tspsd/tsp/random24_tsp_100/", "./data/tspsd/dense_sampling/random24_ov_AVD_100/", "./data/tspsd/dense_sampling/random24_ov_AVD_100/", "./data/tspsd/dense_sampling/random24_ov_AVD_100/"]
LEGENDS = ["HCP (exact)", "HCP-SD (exact)", "HCP-SD (heuristic)", "weak HCP-SD (heuristic)"]
WINDOW_SIZES = [14, 8, 8, 8]
TITLE = "24-vertex graph"
FIG_DIR = "./figures/scp/hamiltonicity/"
calculate_AVD = False
BOUNDS_FILE = "./demo.json"
PROBLEM_TYPE = "all"
COLORS = ["tab:blue", "tab:orange", "gold", "tab:green"]



## Calculating bounds: scp-exact
# LOG_DIRS = ["./log/scp-exact/ham_bound_v1/random10_ov_50/", "./log/scp-exact/ham_bound_v1/random20_ov_50/", "./log/scp-exact/ham_bound_v1/random30_ov_50/", "./log/scp-exact/ham_bound_v1/random40_ov_50/", "./log/scp-exact/ham_bound_v1/random50_ov_50/", "./log/scp-exact/ham_bound_v1/random60_ov_50/", "./log/scp-exact/ham_bound_v1/random70_ov_50/", "./log/scp-exact/ham_bound_v1/random80_ov_50/", "./log/scp-exact/ham_bound_v1/random90_ov_50/", "./log/scp-exact/ham_bound_v1/random100_ov_50/", "./log/scp-exact/ham_bound_v1/random200_ov_50/"]
# PROBLEM_DIRS = ["./data/tspsd/ham_bound_v1/random10_ov_50/", "./data/tspsd/ham_bound_v1/random20_ov_50/", "./data/tspsd/ham_bound_v1/random30_ov_50/", "./data/tspsd/ham_bound_v1/random40_ov_50/", "./data/tspsd/ham_bound_v1/random50_ov_50/", "./data/tspsd/ham_bound_v1/random60_ov_50/", "./data/tspsd/ham_bound_v1/random70_ov_50/", "./data/tspsd/ham_bound_v1/random80_ov_50/", "./data/tspsd/ham_bound_v1/random90_ov_50/", "./data/tspsd/ham_bound_v1/random100_ov_50/", "./data/tspsd/ham_bound_v1/random200_ov_50/"]
# LEGENDS = ["random10", "random20", "random30", "random40", "random50", "random60", "random70", "random80", "random90", "random100", "random200"]
# WINDOW_SIZES = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
# TITLE = "SCP_ham_bound_v1"
# FIG_DIR = "./figures/scp/hamiltonicity/"
# BOUNDS_FILE = "./log/scp-exact/ham_bound_v1/bounds_AVD.json"
# PROBLEM_TYPE = "SCP"
# calculate_AVD = True


## Calculating bounds: wcp-meta
# LOG_DIRS = ["./log/wcp-meta/ham_bound_v1/random10_ov_50_WCP_10s/", "./log/wcp-meta/ham_bound_v1/random20_ov_50_WCP_20s/", "./log/wcp-meta/ham_bound_v1/random30_ov_50_WCP_30s/", "./log/wcp-meta/ham_bound_v1/random40_ov_50_WCP_40s/", "./log/wcp-meta/ham_bound_v1/random50_ov_50_WCP_50s/", "./log/wcp-meta/ham_bound_v1/random60_ov_50_WCP_60s/", "./log/wcp-meta/ham_bound_v1/random70_ov_50_WCP_70s/", "./log/wcp-meta/ham_bound_v1/random80_ov_50_WCP_80s/", "./log/wcp-meta/ham_bound_v1/random90_ov_50_WCP_90s/", "./log/wcp-meta/ham_bound_v1/random100_ov_50_WCP_100s/"]
# PROBLEM_DIRS = ["./data/tspsd/ham_bound_v1/random10_ov_50/", "./data/tspsd/ham_bound_v1/random20_ov_50/", "./data/tspsd/ham_bound_v1/random30_ov_50/", "./data/tspsd/ham_bound_v1/random40_ov_50/", "./data/tspsd/ham_bound_v1/random50_ov_50/", "./data/tspsd/ham_bound_v1/random60_ov_50/", "./data/tspsd/ham_bound_v1/random70_ov_50/", "./data/tspsd/ham_bound_v1/random80_ov_50/", "./data/tspsd/ham_bound_v1/random90_ov_50/", "./data/tspsd/ham_bound_v1/random100_ov_50/", "./data/tspsd/ham_bound_v1/random200_ov_50/"]
# LEGENDS = ["random10", "random20", "random30", "random40", "random50", "random60", "random70", "random80", "random90", "random100", "random200"]
# WINDOW_SIZES = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
# TITLE = "WCP_ham_bound_v1"
# FIG_DIR = "./figures/wcp/hamiltonicity/"
# BOUNDS_FILE = "./log/wcp-meta/ham_bound_v1/bounds_AVD.json"
# PROBLEM_TYPE = "WCP"
# calculate_AVD = True


## Calculating bounds: scp-meta
# LOG_DIRS = ["./log/scp-meta/ham_bound_v1/random10_ov_50_SCP_10s/", "./log/scp-meta/ham_bound_v1/random20_ov_50_SCP_20s/", "./log/scp-meta/ham_bound_v1/random30_ov_50_SCP_30s/", "./log/scp-meta/ham_bound_v1/random40_ov_50_SCP_40s/", "./log/scp-meta/ham_bound_v1/random50_ov_50_SCP_50s/", "./log/scp-meta/ham_bound_v1/random60_ov_50_SCP_60s/", "./log/scp-meta/ham_bound_v1/random70_ov_50_SCP_70s/", "./log/scp-meta/ham_bound_v1/random80_ov_50_SCP_80s/", "./log/scp-meta/ham_bound_v1/random90_ov_50_SCP_90s/", "./log/scp-meta/ham_bound_v1/random100_ov_50_SCP_100s/"]
# PROBLEM_DIRS = ["./data/tspsd/ham_bound_v1/random10_ov_50/", "./data/tspsd/ham_bound_v1/random20_ov_50/", "./data/tspsd/ham_bound_v1/random30_ov_50/", "./data/tspsd/ham_bound_v1/random40_ov_50/", "./data/tspsd/ham_bound_v1/random50_ov_50/", "./data/tspsd/ham_bound_v1/random60_ov_50/", "./data/tspsd/ham_bound_v1/random70_ov_50/", "./data/tspsd/ham_bound_v1/random80_ov_50/", "./data/tspsd/ham_bound_v1/random90_ov_50/", "./data/tspsd/ham_bound_v1/random100_ov_50/", "./data/tspsd/ham_bound_v1/random200_ov_50/"]
# LEGENDS = ["random10", "random20", "random30", "random40", "random50", "random60", "random70", "random80", "random90", "random100", "random200"]
# WINDOW_SIZES = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
# TITLE = "SCP_ham_bound_v1"
# FIG_DIR = "./figures/scp/hamiltonicity/"
# BOUNDS_FILE = "./log/scp-meta/ham_bound_v1/bounds_AVD.json"
# PROBLEM_TYPE = "SCP"
# calculate_AVD = True


DECIMALS = 1
REL_STEP = 0.5

fig_output = FIG_DIR
bound_vd = []
problem_sizes = []
for log_dir, problem_dir, window_size, legend, color1 in zip(LOG_DIRS, PROBLEM_DIRS, WINDOW_SIZES, LEGENDS, COLORS):
    dataset = log_dir.split('/')[-2]
    print(dataset)
    problem_size = int(dataset.split('_')[0].split('random')[1])
    fig_output = fig_output + dataset + ","
    if calculate_AVD:
        degrees, probs = get_probs_in_AVD(problem_dir, log_dir, DECIMALS)
    else:
        step = int(REL_STEP * problem_size)
        if step == problem_size/2:
            degrees, probs = get_probs_in_half(log_dir)
        else:
            degrees, probs = get_probs_in_step(problem_dir, log_dir, step, DECIMALS)
    color = plt.plot(degrees, probs, alpha=1, marker='.', linestyle='none', markersize='2', color=color1)[0].get_color()

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


fig_output = fig_output[:-1] + "_AVD.pdf"
plt.legend()
plt.grid()
plt.xlabel("AVD")
plt.ylabel("Hamiltonian - probability []")
plt.title(TITLE)

if not os.path.exists(FIG_DIR):
    os.makedirs(FIG_DIR)
plt.savefig(fig_output, bbox_inches='tight')
print("Exported " + fig_output)

plt.show()

