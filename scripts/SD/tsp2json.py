import sys
import json

inp = sys.argv[1]
output = sys.argv[2]

# Load .tsp instance to dictionary
data = {}

with open(inp) as input_file:
    lines = input_file.readlines()

node_coord_section = False

for line in lines:
    words = line.split()
    if words[0] == 'NAME:':
        data['NAME'] = words[1]
    elif words[0] == 'TYPE:':
        data['TYPE'] = words[1]
    elif words[0] == 'COMMENT:':
        data['COMMENT:'] = ' '.join(words[1:])
    elif words[0] == 'DIMENSION:':
        data['DIMENSION'] = int(words[1])
    elif words[0] == 'EDGE_WEIGHT_TYPE:':
        data['EDGE_WEIGHT_TYPE'] = words[1]
    elif words[0] == 'NODE_COORD_SECTION':
        node_coord_section = True
        data['NODE_COORDS'] = {}
    elif words[0] == 'EOF':
        node_coord_section = False
    elif node_coord_section:
        data['NODE_COORDS'][words[0]] = [float(x) for x in words[1:]]


# Export dict to .json
with open(output, "w") as output_file:
    json.dump(data, output_file, indent=4)

