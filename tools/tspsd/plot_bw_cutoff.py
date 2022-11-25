#!/usr/bin/python
import os
from tspsd_common import *
import matplotlib.pyplot as plt

# Plots infeasibility detection ratio in different depths
LOG_DIR = "./log/scp-exact/dense_sampling/random24_ov_100_AVD_SCP_v2/"
TITLE = "24-vertex graph"
FIG_OUTPUT = "./figures/scp/hamiltonicity/bw_cutoff_random24_ov_100_AVD.pdf"
MAX_DEPTHS = [1, 2, 3, 5, 10]
SIZE = 24

for max_depth in MAX_DEPTHS:
    EAVDs, infeasible_detected, infeasible_cnts, total_cnts = get_cutoff_success(LOG_DIR, max_depth)
    val_EAVDS = []
    success_rate = []
    for detected, cnt, EAVD in zip(infeasible_detected, infeasible_cnts, EAVDs):
        if cnt != 0:
            success_rate.append(detected/cnt)
            val_EAVDS.append(EAVD)
    plt.plot(val_EAVDS, success_rate, label="abs. depth=" + str(max_depth))


plt.grid()
plt.xlim(0, 12)
plt.xlabel("AVD")
plt.ylabel("detected/infeasible")
plt.title(TITLE)
plt.legend()


plt.savefig(FIG_OUTPUT, bbox_inches='tight')
print("Exported " + FIG_OUTPUT)
plt.show()

