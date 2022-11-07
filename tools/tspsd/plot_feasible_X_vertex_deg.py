#!/usr/bin/python
import os
import matplotlib.pyplot as plt
from tools.tspsd.tspsd_common import get_probs_in_half
import pandas as pd


INPUT_DIRS = [
            "./log/scp-meta/random50_ov_100_5s/",
            # "./log/scp-meta/random_tsp50_100_5s/"
            ]

OUTPUT_DIR = "./figures/scp/hamiltonicity/"

fig_output = OUTPUT_DIR
datasets = []
for input_dir in INPUT_DIRS:
    degrees, probs = get_probs_in_half(input_dir)
    dataset = input_dir.split('/')[-2]
    datasets.append(dataset)
    plt.plot(degrees, probs)
    fig_output = fig_output + dataset + ","

fig_output = fig_output[:-1] + ".pdf"
plt.legend(datasets)

plt.grid()
plt.xlabel("Expected average vertex degree after n/2 steps")
plt.ylabel("Hamiltonian - probability []")

if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)
plt.savefig(fig_output)
print("Exported " + fig_output)

plt.show()
