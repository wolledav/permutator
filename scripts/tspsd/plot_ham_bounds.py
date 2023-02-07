#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy as np
from tspsd_common import *

plt.figure(figsize=[6.4, 3.6])

BOUND_FILES = [
    "./log/TSPSD-exact/ham_bound_v1/bounds_AVD.json",
    "./log/TSPSD-meta/ham_bound_v1/bounds_AVD.json",
    "./log/wTSPSD-meta/ham_bound_v1/bounds_AVD.json",


]

LEGENDS = [
    "HCP-SD (exact)",
    "HCP-SD (heuristic)",
    "weak HCP-SD (heuristic)",
]

# COLORS = ["gold", "tab:orange", "darkgoldenrod", "greenyellow", "tab:green", "darkgreen"]
COLORS = ["tab:orange", "gold", "tab:green"]

FIG_OUTPUT = "./figures/TSPSD/hamiltonicity/HCPxTSPSDxwTSPSD_bounds_v4.pdf"
TITLE = "(b) random datasets with 10-200 vertices"


# Plot TSP bound
sizes = [*range(10, 201, 1)]
bounds = [np.log(v) + np.log(np.log(v)) for v in sizes]
plt.plot(sizes, bounds, label="HCP (Komlós-Szemerédi bound)", color="tab:blue")


# Plot bound from experiments
for bound_file, color, legend in zip(BOUND_FILES, COLORS, LEGENDS):
    bound_data = get_data(bound_file)
    sizes = bound_data["sizes"]
    bounds = bound_data["bounds"]
    legend = legend
    plt.plot(sizes, bounds, label=legend, marker='.', color=color)

plt.legend()
plt.xlabel("|V|")
plt.ylabel("AVD")
plt.title(TITLE)
plt.grid()

plt.savefig(FIG_OUTPUT, bbox_inches='tight')
print("Exported " + FIG_OUTPUT)

plt.show()
