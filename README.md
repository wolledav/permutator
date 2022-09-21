# Permutator

Clone the repository. Then, go to the permutator/ directory and run

### Compilation
```
cmake -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles" -S . -B"./cmake-build-release"
cmake --build cmake-build-release --target all -- -j 6
```

### Usage
In the permutator/ directory, run
```
./cmake-build-release/scp_meta -d ./data/tspsd/berlin52_distance/berlin52_sd1_distance.json -o ./log/scp-meta/solution.out -t 10
```
or
```
./cmake-build-release/wcp_meta -d ./data/tspsd/berlin52_distance/berlin52_sd1_distance.json -o ./log/wcp-meta/solution.out -t 10
```
Parameters:

-d . . . path to a problem instance

-o . . . path to an output file (optional)

-t . . . timeout is seconds (optional)

-c . . . solver configuration file (optional)


### Gurobi installation
1) Download and extract Gurobi to /opt
2) Add to ~/.bashrc:
```
export GUROBI_HOME="/opt/gurobi951/linux64"
export PATH="${PATH}:${GUROBI_HOME}/bin"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${GUROBI_HOME}/lib"
```
3) Run
```
cd /opt/gurobi951/linux64/src/build
make
cp libgurobi_c++.a ../../lib/
```


## Components overview

### Metaheuristics
| Name            | Parameters                            |
|-----------------|---------------------------------------|
| ILS             | ils_k                                 |
| basic VNS       | bvns_min_k, bvns_max_k                |
| calibrating VNS | cvns_min_k, cvns_max_k, cvns_it_per_k |

### Perturbations
| Name            | Parameters     |
|-----------------|----------------|
| double_bridge   | k, reverse_all |
| reinsert        | k              |
| random_swap     | k              |
| random_move     | k              |
| random_move_all | k              |

### Local search heuristics
| Name            | Parameters | Neighborhood order |
|-----------------|------------|--------------------|
| basic VND       | none       | fixed              |
| pipe VND        | none       | fixed              |
| cyclic VND      | none       | fixed              |
| random VND      | none       | random             |
| random pipe VND | none       | random             |

### Local search operators
| Name              | Parameters    |
|-------------------|---------------|
| insert1           | none          |
| remove1           | none          |
| relocate          | x, reverse    |
| exchange          | x, y, reverse |
| centered exchange | x             |
| move_all          | x             |
| exchange_ids      | none          |
| exchange_n_ids    | none          |

### Construction procedures
| Name             | Parameters |
|------------------|------------|
| greedy           | none       |
| random           | none       |
| random replicate | none       |


