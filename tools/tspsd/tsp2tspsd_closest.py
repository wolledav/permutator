import sys
import json
import pprint
import random
import numpy as np


inp = sys.argv[1]
output_dir = sys.argv[2]
k = int(sys.argv[3])

# Load .tsp instance from json
with open(inp) as input_file:
    data = json.load(input_file)

# Remove k closest edges for each node
data['DELETE'] = {}
dim = data['DIMENSION']

for node1 in data['NODE_COORDS']:
    data['DELETE'][node1] = []
    dists = {}
    for node2 in data['NODE_COORDS']:
        if node1 != node2:
            dist = np.linalg.norm(np.array(data['NODE_COORDS'][node1]) - np.array(data['NODE_COORDS'][node2]))
            dists[dist] = node2
    for dist in sorted(dists.keys())[:k]:
        data['DELETE'][node1].append(dists[dist])

# Export
name = inp.split('/')[-1][:-5]
output = output_dir + name + "_sd" + str(k) + "_cl.json"
data['TYPE'] = "TSPSD"

with open(output, "w") as output_file:
    json.dump(data, output_file, indent=4)
    print('Exported ' + output)
