#!/usr/bin/python
import os
from tspsd_common import *
import matplotlib.pyplot as plt

LOG_DIRS = ["./log/scp-exact/ham_bound_v1/random10_ov_50/", "./log/scp-exact/ham_bound_v1/random20_ov_50/", "./log/scp-exact/ham_bound_v1/random30_ov_50/", "./log/scp-exact/ham_bound_v1/random40_ov_50/", "./log/scp-exact/ham_bound_v1/random50_ov_50/", "./log/scp-exact/ham_bound_v1/random60_ov_50/", "./log/scp-exact/ham_bound_v1/random70_ov_50/", "./log/scp-exact/ham_bound_v1/random80_ov_50/", "./log/scp-exact/ham_bound_v1/random90_ov_50/", "./log/scp-exact/ham_bound_v1/random100_ov_50/"]
SIZES = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
MAX_DEPTHS = [0, 1, 2, 3, 4, 5, 10]
TITLE = "random10-100_ov_50"
FIG_OUTPUT = "./figures/scp/hamiltonicity/bw_multi_cutoffs_" + TITLE + ".pdf"


for max_depth in MAX_DEPTHS:
    V_sizes = []
    success_rates = []
    for log_dir, size in zip(LOG_DIRS, SIZES):
        EAVDs, infeasible_detected, infeasible_cnts = get_cutoff_success(log_dir, max_depth)
        V_sizes.append(size)
        success_rates.append(sum(infeasible_detected)/sum(infeasible_cnts))
    print(max_depth)
    print(V_sizes)
    print(success_rates)
    plt.plot(V_sizes, success_rates, marker='.', label="BW search depth=" + str(max_depth))

plt.title(TITLE)
plt.xlabel("|V|")
plt.ylabel("detected/infeasible")
plt.grid()
plt.legend()

plt.savefig(FIG_OUTPUT, bbox_inches='tight')
print("Exported " + FIG_OUTPUT)
plt.show()
