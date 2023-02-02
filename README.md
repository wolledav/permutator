## Permutator 
### generic metaheuristic solver for problems with permutative representation 

---

**Maintaners:** [David Woller](http://imr.ciirc.cvut.cz/People/David)

**Creators:** [David Woller](http://imr.ciirc.cvut.cz/People/David), Jan Hrazdíra

**Contributors:** [David Woller](http://imr.ciirc.cvut.cz/People/David), Jan Hrazdíra, David Kolečkář, Jan Švrčina

**Contact:** wolledav@ciirc.cvut.cz

---

## Introduction

The **permutator** solver implements several highly configurable metaheuristic algorithms that can be readily applied to a large class of problems with permutative solution representation.
A problem solution must be possible to encode as an ordered sequence of nodes.
The sequence length can be variable, and individual nodes can reoccur.
Additional problem-specific properties and constraints are treated using a penalization mechanism.

**Keywords:** permutative representation, metaheuristics, Iterated Local Search, Variable Neighborhood Search

**Relevant literature:**

1. Woller, D., Hrazdíra, J., Kulich, M. (2023). Metaheuristic Solver for Problems with Permutative Representation. Intelligent Computing & Optimization. ICO 2022. Lecture Notes in Networks and Systems, vol 569. Springer, Cham. https://doi.org/10.1007/978-3-031-19958-5_5




## Usage

Clone or download the repository. 

```
git clone git@github.com:wolledav/permutator.git
```


### Compilation

Go to the permutator/ directory and run

```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B"./cmake-build-release_"
cmake --build cmake-build-release_ -j
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B"./cmake-build-debug_"
cmake --build cmake-build-debug_ -j
```

### Usage
In the permutator/ directory, run
```
./cmake-build-debug_/scp_meta -d ./data_demo/datasets/TSPSD/berlin52-13.2.json
```
or
```
./cmake-build-release_/wcp_meta -d ./data_demo/datasets/TSPSD/berlin52-13.2.json -i ./data_demo/results/TSPSD/berlin52-13.2_init.json -o ./data_demo/results/wTSPSD/berlin52-13.2.json  -c ./configs/SCP_config_opt.json -t 60
```
Parameters:

-d . . . path to a problem instance

-o . . . path to an output file (optional)

-i . . . initial solution (optional)

-t . . . timeout in seconds (optional)

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
| doubleBridge   | k, reverse_all |
| reinsert        | k              |
| randomSwap     | k              |
| randomMove     | k              |
| randomMoveAll | k              |

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
| moveAll          | x             |
| exchangeIds      | none          |
| exchangeNIds    | none          |

### Construction procedures
| Name             | Parameters |
|------------------|------------|
| greedy           | none       |
| random           | none       |
| random replicate | none       |


