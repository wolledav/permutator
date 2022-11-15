#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy as np
from tspsd_common import *

BOUND_FILES = [
    "./log/scp-exact/ham_bound_v1/bounds_0.25|V|.json",
    "./log/scp-exact/ham_bound_v1/bounds_0.5|V|.json",
    "./log/scp-exact/ham_bound_v1/bounds_0.75|V|.json",
    "./log/wcp-meta/ham_bound_v1/bounds_0.25|V|.json",
    "./log/wcp-meta/ham_bound_v1/bounds_0.5|V|.json",
    "./log/wcp-meta/ham_bound_v1/bounds_0.75|V|.json"
               ]
COLORS = ["gold", "tab:orange", "darkgoldenrod", "greenyellow", "tab:green", "darkgreen"]

FIG_OUTPUT = "./figures/scp/hamiltonicity/HCPxSCPxWCP_bounds_v2.pdf"



# Plot TSP bound
sizes = [*range(10, 101, 1)]
bounds = [np.log(v) + np.log(np.log(v)) for v in sizes]
plt.plot(sizes, bounds, label="HCP - Komlós-Szemerédi bound", color="tab:blue")


# Plot bound from experiments
for bound_file, color in zip(BOUND_FILES, COLORS):
    bound_data = get_data(bound_file)
    sizes = bound_data["sizes"]
    bounds = bound_data["bounds"]
    legend = bound_data["problem_type"] + " bound (" + str(bound_data["rel_step"]) + "|V| steps)"
    plt.plot(sizes, bounds, label=legend, marker='.', color=color)

plt.legend()
plt.xlabel("|V|")
plt.ylabel("EAVD")
plt.grid()

plt.savefig(FIG_OUTPUT, bbox_inches='tight')
print("Exported " + FIG_OUTPUT)

plt.show()
