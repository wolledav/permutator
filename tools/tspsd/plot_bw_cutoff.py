#!/usr/bin/python
import os
from tspsd_common import *
import matplotlib.pyplot as plt

# Plots infeasibility detection ratio in different depths
LOG_DIR = "./log/scp-exact/dense_sampling/random24_ov_100_SCP_v2/"
TITLE = "random24_ov_100"
FIG_OUTPUT = "./figures/scp/hamiltonicity/bw_cutoff_" + TITLE + ".pdf"
MAX_DEPTHS = [0, 1, 2, 3, 4, 5, 10, 24]
SIZE = 24

for max_depth in MAX_DEPTHS:
    EAVDs, infeasible_detected, infeasible_cnts = get_cutoff_success(LOG_DIR, max_depth)
    success_rate = []
    for detected, cnt in zip(infeasible_detected, infeasible_cnts):
        if cnt != 0:
            success_rate.append(detected/cnt)
        else:
            success_rate.append(0)
    plt.plot(EAVDs, success_rate, label="BW search depth=" + str(max_depth))


plt.grid()
plt.xlabel("EAVD")
plt.ylabel("detected/infeasible")
plt.title(TITLE)
plt.legend()


plt.savefig(FIG_OUTPUT, bbox_inches='tight')
print("Exported " + FIG_OUTPUT)
plt.show()
