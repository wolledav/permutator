# USAGE
# python3 ./tools/draw_boxplots.py ./log/CVRP-meta/test/A-n65-k09/

import os
import sys
import json
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt



log_dir = sys.argv[1]
logs = os.listdir(log_dir)

data_dict = defaultdict(list)

for log in logs:
    path = log_dir + log
    f = open(path)
    data = json.load(f)
    d = data["histogram"]["rate"]
    for key, value in d.items():
        data_dict[key].append(100 * value)

marklist = sorted(data_dict.items(), key=lambda x:-np.median(x[1]))
sorted_data_dict = dict(marklist)

instance = log_dir.split('/')[-2]


fig, ax = plt.subplots()
fig.set_figwidth(8)
fig.set_figheight(6)


bp = ax.boxplot(sorted_data_dict.values(), labels=sorted_data_dict.keys(), showfliers=False)

plt.grid()
plt.subplots_adjust(left=None, bottom=0.28, right=None, top=None, wspace=None, hspace=None)
plt.xticks(rotation=45, ha='right', rotation_mode='anchor')
plt.ylabel('success rate (%)')
plt.title(instance)

plt.savefig('./figures/' + instance + '.pdf')

plt.show()

