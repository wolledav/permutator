#!/usr/bin/python

import os
import json
import matplotlib.pyplot as plt

INPUT_DIR = "./data/tspsd/random24_ov_1/"

total_removed = []
distinct_removed_half = []

files = os.listdir(INPUT_DIR)
for filename in sorted(files):
    path = INPUT_DIR + filename
    with open(path) as f:
        data = json.load(f)
        total_removed.append(data["TOTAL_REMOVED"])
        distinct_removed_half.append(data["DISTINCT_REMOVED_HALF"])

# plt.plot(total_removed, distinct_removed_half, 'x', markersize=0.5)
# plt.xlabel('Removed edges - total')
# plt.ylabel('Distinct removed edges - half')
# plt.grid()
# plt.show()

distinct_int = [int(i) for i in distinct_removed_half]
print(sorted(distinct_int))
plt.hist(distinct_int)
plt.xlabel('Distinct removed edges - half')
plt.ylabel('Instances cnt')
plt.grid()
plt.show()
