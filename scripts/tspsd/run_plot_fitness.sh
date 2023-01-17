#!/bin/bash
# run from permutator/

python3 ./tools/tspsd/plot_fitness.py -i ./log/scp-meta/berlin52_closest_1min/ -o ./figures/scp/ -t SCP
python3 ./tools/tspsd/plot_fitness.py -i ./log/scp-meta/berlin52_random_1min/ -o ./figures/scp/ -t SCP

python3 ./tools/tspsd/plot_fitness.py -i ./log/scp-meta/berlin52_closest_10min/ -o ./figures/scp/ -t SCP
python3 ./tools/tspsd/plot_fitness.py -i ./log/scp-meta/berlin52_random_10min/ -o ./figures/scp/ -t SCP


python3 ./tools/tspsd/plot_fitness.py -i ./log/wcp-meta/berlin52_closest_1min/ -o ./figures/wcp/ -t WCP
python3 ./tools/tspsd/plot_fitness.py -i ./log/wcp-meta/berlin52_random_1min/ -o ./figures/wcp/ -t WCP

python3 ./tools/tspsd/plot_fitness.py -i ./log/wcp-meta/berlin52_closest_10min/ -o ./figures/wcp/ -t WCP
python3 ./tools/tspsd/plot_fitness.py -i ./log/wcp-meta/berlin52_random_10min/ -o ./figures/wcp/ -t WCP
