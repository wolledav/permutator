import sys
import json

output_file = sys.argv[1]
params = sys.argv[2:]
operators = []
perturbations = []
config = {}

for i in range(0, len(params), 2):
    param = params[i]
    value = params[i + 1]

    if param[0:3] == "op_":
        if value == '1':
            operators.append(param[3:])
    elif param == "perturbation":
        perturbations.append(value)
    elif param == "allow_infeasible":
        config[param] = bool(int(value))
    elif param == "ils_k":
        config[param] = int(value)
    elif param == "bvns_min_k":
        config[param] = int(value)
    elif param == "bvns_max_k":
        config[param] = int(value)
    elif param == "construction":
        config[param] = value
    elif param == "metaheuristic":
        config[param] = value
    elif param == "local_search":
        config[param] = value
    elif param == "timeout":
        config[param] = int(value)
    elif param == "num_threads":
        config[param] = int(value)
    elif param == "stop_on_feasible":
        config[param] = bool(int(value))
    elif param == "first_improve":
        config[param] = bool(int(value))
    else:
        print(sys.argv[0], "unknown parameter:", param)

config["operators"] = operators
config["perturbation"] = perturbations

file = open(output_file, "w")
json.dump(config, file, indent=4)
file.close()