#!/usr/bin/python
import os
from tspsd_common import *
import matplotlib.pyplot as plt

LOG_DIRS = ["./log/scp-exact/ham_bound_v1/random10_ov_50/", "./log/scp-exact/ham_bound_v1/random20_ov_50/", "./log/scp-exact/ham_bound_v1/random30_ov_50/", "./log/scp-exact/ham_bound_v1/random40_ov_50/", "./log/scp-exact/ham_bound_v1/random50_ov_50/", "./log/scp-exact/ham_bound_v1/random60_ov_50/", "./log/scp-exact/ham_bound_v1/random70_ov_50/", "./log/scp-exact/ham_bound_v1/random80_ov_50/", "./log/scp-exact/ham_bound_v1/random90_ov_50/", "./log/scp-exact/ham_bound_v1/random100_ov_50/", "./log/scp-exact/ham_bound_v1/random200_ov_50/"]
SIZES = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200]
REL_DEPTHS = [0, 0.1, 0.25, 0.5, 1]
TITLE = ""
FIG_OUTPUT = "./figures/scp/hamiltonicity/bw_multi_cutoffs_random10-200_ov_50_AVD_rel.pdf"


for rel_depth in REL_DEPTHS:
    V_sizes = []
    success_rates = []
    for log_dir, size in zip(LOG_DIRS, SIZES):
        max_depth = int(rel_depth * size)
        EAVDs, infeasible_detected_cnts, infeasible_cnts, total_cnts = get_cutoff_success(log_dir, max_depth)

        id_sum = 0
        i_sum = 0
        for id_cnt, i_cnt, t_cnt in zip(infeasible_detected_cnts, infeasible_cnts, total_cnts):
            if i_cnt < t_cnt: # if not all instances infeasible
                id_sum += id_cnt
                i_sum += i_cnt
        V_sizes.append(size)
        success_rates.append(id_sum/i_sum)
        print(size, id_sum, sum(infeasible_detected_cnts), i_sum, sum(infeasible_cnts))
    print(max_depth)
    print(V_sizes)
    print(success_rates)
    plt.plot(V_sizes, success_rates, marker='.', label="BW search rel. depth=" + str(rel_depth) + "|V|")

plt.title(TITLE)
plt.xlabel("|V|")
plt.ylabel("detected/infeasible")
plt.grid()
plt.legend()

plt.savefig(FIG_OUTPUT, bbox_inches='tight')
print("Exported " + FIG_OUTPUT)
plt.show()
