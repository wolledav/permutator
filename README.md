# Permutator

## Usage

### Compilation
```
cd path/to/permutator
cmake -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles" -S . -B"./cmake-build-release"
cmake --build cmake-build-release --target all -- -j 6
```

### Gurobi installation
1) Download and extract Gurobi in /opt
2) Add to ~/.bashrc
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
| Name            | Description |
|-----------------|-------------|
| ILS             |             |
| basic VNS       |             |
| calibrating VNS |             |

### Perturbations
| Name                          | Description |
|-------------------------------|-------------|
| double_bridge(k, reverse_all) |             |
| reinsert(k)                   |             |
| random_swap(k)                |             |
| random_move(k)                |             |
| random_move_all(k)            |             |

### Local search heuristics
| Name            | Description |
|-----------------|-------------|
| basic VND       |             |
| pipe VND        |             |
| cyclic VND      |             |
| random VND      |             |
| random pipe VND |             |

### Local search operators
| Name                    | Description |
|-------------------------|-------------|
| insert1                 |             |
| remove1                 |             |
| relocate(x, reverse)    |             |
| exchange(x, y, reverse) |             |
| centered exchange(x)    |             |
| move_all(x)             |             |
| exchange_ids            |             |
| exchange_n_ids          |             |

### Construction procedures
| Name             | Description |
|------------------|-------------|
| greedy           |             |
| random           |             |
| random replicate |             |


