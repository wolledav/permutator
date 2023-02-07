#!/bin/bash
# run from permutator/

python3 ./tools/tspsd/plot_fitness.py -i ./log/TSPSD-meta/berlin52_closest_1min/ -o ./figures/TSPSD/ -t TSPSD
python3 ./tools/tspsd/plot_fitness.py -i ./log/TSPSD-meta/berlin52_random_1min/ -o ./figures/TSPSD/ -t TSPSD

python3 ./tools/tspsd/plot_fitness.py -i ./log/TSPSD-meta/berlin52_closest_10min/ -o ./figures/TSPSD/ -t TSPSD
python3 ./tools/tspsd/plot_fitness.py -i ./log/TSPSD-meta/berlin52_random_10min/ -o ./figures/TSPSD/ -t TSPSD


python3 ./tools/tspsd/plot_fitness.py -i ./log/wTSPSD-meta/berlin52_closest_1min/ -o ./figures/wTSPSD/ -t wTSPSD
python3 ./tools/tspsd/plot_fitness.py -i ./log/wTSPSD-meta/berlin52_random_1min/ -o ./figures/wTSPSD/ -t wTSPSD

python3 ./tools/tspsd/plot_fitness.py -i ./log/wTSPSD-meta/berlin52_closest_10min/ -o ./figures/wTSPSD/ -t wTSPSD
python3 ./tools/tspsd/plot_fitness.py -i ./log/wTSPSD-meta/berlin52_random_10min/ -o ./figures/wTSPSD/ -t wTSPSD
