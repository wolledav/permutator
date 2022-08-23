import sys
import json
import pprint
import random

inp = sys.argv[1]
output_dir = sys.argv[2]
k = int(sys.argv[3])

# Load .tsp instance from json
with open(inp) as input_file:
    data = json.load(input_file)

# Remove k random edges for each node
data['DELETE'] = {}
dim = data['DIMENSION']

for node in data['NODE_COORDS']:
    data['DELETE'][node] = []
    for i in range(k):
        deleted = random.randint(1, dim)
        while deleted == node:
            deleted = random.randint(1, dim)
        data['DELETE'][node].append(str(deleted))

# Export
name = inp.split('/')[-1][:-5]
output = output_dir + name + "_sd" + str(k) + ".json"
data['TYPE'] = "TSPSD"

with open(output, "w") as output_file:
    json.dump(data, output_file, indent=4)
    print('Exported ' + output)
