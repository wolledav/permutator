#!/usr/bin/python
import matplotlib.pyplot as plt
from tools.tspsd.tspsd_common import *
import pandas as pd

# Hamiltonicity on random24 dataset
LOG_DIRS = [ "./log/TSPSD-exact/dense_sampling/random24_tsp_100_HCP/", "./log/TSPSD-exact/dense_sampling/random24_ov_100_AVD_TSPSD/", "./log/TSPSD-meta/dense_sampling/random24_ov_AVD_100_TSPSD_24s/", "./log/wTSPSD-meta/dense_sampling/random24_ov_AVD_100_wTSPSD_24s/"]
PROBLEM_DIRS = ["./data/tspsd/tsp/random24_tsp_100/", "./data/tspsd/dense_sampling/random24_ov_AVD_100/", "./data/tspsd/dense_sampling/random24_ov_AVD_100/", "./data/tspsd/dense_sampling/random24_ov_AVD_100/"]
LEGENDS = ["HCP (exact)", "HCP-SD (exact)", "HCP-SD (heuristic)", "weak HCP-SD (heuristic)"]
WINDOW_SIZES = [14, 8, 8, 8]
TITLE = "(a) random24-100 dataset"
FIG_DIR = "./figures/TSPSD/hamiltonicity/"
calculate_AVD = False
BOUNDS_FILE = "./demo.json"
PROBLEM_TYPE = "all"
COLORS = ["tab:blue", "tab:orange", "gold", "tab:green"]



# Calculating bounds: TSPSD-exact
# # LOG_DIRS = ["./log/TSPSD-exact/ham_bound_v1/random10_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random20_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random30_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random40_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random50_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random60_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random70_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random80_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random90_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random100_ov_50/", "./log/TSPSD-exact/ham_bound_v1/random200_ov_50/"]
# LOG_DIRS = ["./log/TSPSD-exact/ham_bound_v1/random150_ov_50_v2/"]
# # PROBLEM_DIRS = ["./data/tspsd/ham_bound_v1/random10_ov_50/", "./data/tspsd/ham_bound_v1/random20_ov_50/", "./data/tspsd/ham_bound_v1/random30_ov_50/", "./data/tspsd/ham_bound_v1/random40_ov_50/", "./data/tspsd/ham_bound_v1/random50_ov_50/", "./data/tspsd/ham_bound_v1/random60_ov_50/", "./data/tspsd/ham_bound_v1/random70_ov_50/", "./data/tspsd/ham_bound_v1/random80_ov_50/", "./data/tspsd/ham_bound_v1/random90_ov_50/", "./data/tspsd/ham_bound_v1/random100_ov_50/", "./data/tspsd/ham_bound_v1/random200_ov_50/"]
# PROBLEM_DIRS = ["./data/tspsd/ham_bound_v1/random150_ov_50_v2/"]
# # LEGENDS = ["random10", "random20", "random30", "random40", "random50", "random60", "random70", "random80", "random90", "random100", "random200"]
# LEGENDS = ["random150"]
# # WINDOW_SIZES = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
# WINDOW_SIZES = [2]
# TITLE = "TSPSD_ham_bound_v1"
# FIG_DIR = "./figures/TSPSD/hamiltonicity/"
# BOUNDS_FILE = "./log/TSPSD-exact/ham_bound_v1/bounds_AVD_new.json"
# PROBLEM_TYPE = "TSPSD"
# calculate_AVD = True


# # Calculating bounds: wTSPSD-meta
# # LOG_DIRS = ["./log/wTSPSD-meta/ham_bound_v1/random10_ov_50_wTSPSD_10s/", "./log/wTSPSD-meta/ham_bound_v1/random20_ov_50_wTSPSD_20s/", "./log/wTSPSD-meta/ham_bound_v1/random30_ov_50_wTSPSD_30s/", "./log/wTSPSD-meta/ham_bound_v1/random40_ov_50_wTSPSD_40s/", "./log/wTSPSD-meta/ham_bound_v1/random50_ov_50_wTSPSD_50s/", "./log/wTSPSD-meta/ham_bound_v1/random60_ov_50_wTSPSD_60s/", "./log/wTSPSD-meta/ham_bound_v1/random70_ov_50_wTSPSD_70s/", "./log/wTSPSD-meta/ham_bound_v1/random80_ov_50_wTSPSD_80s/", "./log/wTSPSD-meta/ham_bound_v1/random90_ov_50_wTSPSD_90s/", "./log/wTSPSD-meta/ham_bound_v1/random100_ov_50_wTSPSD_100s/", "./log/wTSPSD-meta/ham_bound_v1/random200_ov_50_wTSPSD_200s/"]
# LOG_DIRS = ["./log/wTSPSD-meta/ham_bound_v1/random150_ov_50_v2_wTSPSD_150s/"]
# # PROBLEM_DIRS = ["./data/tspsd/ham_bound_v1/random10_ov_50/", "./data/tspsd/ham_bound_v1/random20_ov_50/", "./data/tspsd/ham_bound_v1/random30_ov_50/", "./data/tspsd/ham_bound_v1/random40_ov_50/", "./data/tspsd/ham_bound_v1/random50_ov_50/", "./data/tspsd/ham_bound_v1/random60_ov_50/", "./data/tspsd/ham_bound_v1/random70_ov_50/", "./data/tspsd/ham_bound_v1/random80_ov_50/", "./data/tspsd/ham_bound_v1/random90_ov_50/", "./data/tspsd/ham_bound_v1/random100_ov_50/", "./data/tspsd/ham_bound_v1/random200_ov_50/"]
# PROBLEM_DIRS = ["./data/tspsd/ham_bound_v1/random150_ov_50_v2/"]
# # LEGENDS = ["random10", "random20", "random30", "random40", "random50", "random60", "random70", "random80", "random90", "random100", "random200"]
# LEGENDS = ["random150"]
# # WINDOW_SIZES = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
# WINDOW_SIZES = [2]
# TITLE = "wTSPSD_ham_bound_v1"
# FIG_DIR = "./figures/wTSPSD/hamiltonicity/"
# BOUNDS_FILE = "./log/wTSPSD-meta/ham_bound_v1/bounds_AVD_new.json"
# PROBLEM_TYPE = "wTSPSD"
# calculate_AVD = True


# Calculating bounds: TSPSD-meta
# LOG_DIRS = ["./log/TSPSD-meta/ham_bound_v1/random10_ov_50_TSPSD_10s/", "./log/TSPSD-meta/ham_bound_v1/random20_ov_50_TSPSD_20s/", "./log/TSPSD-meta/ham_bound_v1/random30_ov_50_TSPSD_30s/", "./log/TSPSD-meta/ham_bound_v1/random40_ov_50_TSPSD_40s/", "./log/TSPSD-meta/ham_bound_v1/random50_ov_50_TSPSD_50s/", "./log/TSPSD-meta/ham_bound_v1/random60_ov_50_TSPSD_60s/", "./log/TSPSD-meta/ham_bound_v1/random70_ov_50_TSPSD_70s/", "./log/TSPSD-meta/ham_bound_v1/random80_ov_50_TSPSD_80s/", "./log/TSPSD-meta/ham_bound_v1/random90_ov_50_TSPSD_90s/", "./log/TSPSD-meta/ham_bound_v1/random100_ov_50_TSPSD_100s/", "./log/TSPSD-meta/ham_bound_v1/random150_ov_50_TSPSD_150s/"]
# LOG_DIRS = ["./log/TSPSD-meta/ham_bound_v1/random200_ov_50_v2_TSPSD_200s/"]
# PROBLEM_DIRS = ["./data/tspsd/ham_bound_v1/random10_ov_50/", "./data/tspsd/ham_bound_v1/random20_ov_50/", "./data/tspsd/ham_bound_v1/random30_ov_50/", "./data/tspsd/ham_bound_v1/random40_ov_50/", "./data/tspsd/ham_bound_v1/random50_ov_50/", "./data/tspsd/ham_bound_v1/random60_ov_50/", "./data/tspsd/ham_bound_v1/random70_ov_50/", "./data/tspsd/ham_bound_v1/random80_ov_50/", "./data/tspsd/ham_bound_v1/random90_ov_50/", "./data/tspsd/ham_bound_v1/random100_ov_50/", "./data/tspsd/ham_bound_v1/random150_ov_50/"]
# PROBLEM_DIRS = ["./data/tspsd/ham_bound_v1/random200_ov_50_v2/"]
# LEGENDS = ["random10", "random20", "random30", "random40", "random50", "random60", "random70", "random80", "random90", "random100", "random150"]
# LEGENDS = ["random200"]
# WINDOW_SIZES = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4]
# WINDOW_SIZES = [14]
# TITLE = "TSPSD_ham_bound_v1"
# FIG_DIR = "./figures/TSPSD/hamiltonicity/"
# BOUNDS_FILE = "./log/TSPSD-meta/ham_bound_v1/bounds_AVD_new.json"
# PROBLEM_TYPE = "TSPSD"
# calculate_AVD = True


DECIMALS = 1
REL_STEP = 0.5

fig_output = FIG_DIR
bound_vd = []
problem_sizes = []
for log_dir, problem_dir, window_size, legend, color in zip(LOG_DIRS, PROBLEM_DIRS, WINDOW_SIZES, LEGENDS, COLORS):
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
    # color = plt.plot(degrees, probs, alpha=1, marker='.', linestyle='none', markersize='2')[0].get_color()
    plt.plot(degrees, probs, alpha=1, marker='.', linestyle='none', markersize='2', color=color)[0].get_color()


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

