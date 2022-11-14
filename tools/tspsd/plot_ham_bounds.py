#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy as np
from tspsd_common import *

BOUND_FILES = [
    "./log/scp-meta/ham_bound_v1/bounds_0.25n.json",
    "./log/scp-meta/ham_bound_v1/bounds_0.5n.json",
    "./log/scp-meta/ham_bound_v1/bounds_0.75n.json"
               ]
FIG_OUTPUT = "./figures/scp/hamiltonicity/TSPxSCPxWCP_bounds_v1.pdf"

# Plot TSP bound
sizes = [*range(10, 110, 1)]
bounds = [np.log(v) + np.log(np.log(v)) for v in sizes]
plt.plot(sizes, bounds, label="TSP - Komlós-Szemerédi bound")


# Plot bound from experiments
for bound_file in BOUND_FILES:
    bound_data = get_data(bound_file)
    sizes = bound_data["sizes"]
    bounds = bound_data["bounds"]
    legend = bound_data["problem_type"] + " bound (" + str(bound_data["rel_step"]) + "n steps)"
    plt.plot(sizes, bounds, label=legend, marker='.')

plt.legend()
plt.xlabel("|V|")
plt.ylabel("EAVD")
plt.grid()

plt.savefig(FIG_OUTPUT, bbox_inches='tight')
print("Exported " + FIG_OUTPUT)

plt.show()
