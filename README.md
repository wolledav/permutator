## Permutator 
### metaheuristic solver for problems with permutative representation 

**Maintaners:** [David Woller](http://imr.ciirc.cvut.cz/People/David)

**Creators:** [David Woller](http://imr.ciirc.cvut.cz/People/David), Jan Hrazdíra

**Contributors:** [David Woller](http://imr.ciirc.cvut.cz/People/David), Jan Hrazdíra, David Kolečkář, Jan Švrčina

**Contact:** wolledav@ciirc.cvut.cz


## Introduction

The **permutator** solver implements several highly configurable metaheuristic algorithms that can be readily applied to a large class of problems with permutative solution representation.
A problem solution must be possible to encode as an ordered sequence of nodes.
The sequence length can be variable, and individual nodes can reoccur.
Additional problem-specific properties and constraints are treated using a penalization mechanism.

Currently, the following **problems** are formulated: 

- Capacitated Vehicle Routing Problem (CVRP)
- Non-Permutation Flowshop Scheduling (NPFS)
- Quadratic Assignment Problem (QAP)
- Sudoku
- Travelling Salesman Problem on Self-Deleting Graphs (TSPSD)
- weak Travelling Salesman Problem on Self-Deleting Graphs (weak TSPSD)

All **datasets** and experimental **results** are stored separately in a publicly available [Google Drive repository](https://drive.google.com/drive/folders/1BAbfwAIO1iAvtP9s4yG5JJ_jpsE_qtyS?usp=sharing).


**Keywords:** permutative representation, metaheuristics, Iterated Local Search, Variable Neighborhood Search

**Relevant literature:**

1. Woller, D., Hrazdíra, J., Kulich, M. (2023). Metaheuristic Solver for Problems with Permutative Representation. Intelligent Computing & Optimization. ICO 2022. Lecture Notes in Networks and Systems, vol 569. Springer, Cham. https://doi.org/10.1007/978-3-031-19958-5_5




## Usage

Clone or download the repository. 

```
git clone git@github.com:wolledav/permutator.git
```


### Compilation

In the permutator/ directory, run

```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B"./cmake-build-release"
cmake --build cmake-build-release -j
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B"./cmake-build-debug"
cmake --build cmake-build-debug -j
```

### Usage
Parameters:

-d . . . path to a problem instance

-o . . . path to an output file (optional)

-i . . . initial solution (optional)

-t . . . timeout in seconds (optional)

-c . . . solver configuration file (optional)


In the permutator/ directory, run
```
./cmake-build-debug/TSPSD_meta -d ./data_demo/datasets/TSPSD/berlin52-13.2.json
```
The solver prints details about the search progress to the console in debug mode.

To run in release mode and with all optional parameters, run

```
./cmake-build-release/wTSPSD_meta -d ./data_demo/datasets/TSPSD/berlin52-13.2.json -i ./data_demo/results/TSPSD/berlin52-13.2_init.json -o ./data_demo/results/wTSPSD/berlin52-13.2.json  -c ./configs/TSPSD_config_opt.json -t 60
```








## Components overview

The solver implements several metaheuristic algorithms, perturbations, local search heuristics, local search operators and construction procedures.

### Metaheuristics
| Name            | Parameters                            |
|-----------------|---------------------------------------|
| ILS             | ils_k                                 |
| basic VNS       | bvns_min_k, bvns_max_k                |

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



## Tuning

The **permutator** can be automatically tuned to achieve optimal performance on a given problem and dataset.
For this purpose, the [irace](https://mlopez-ibanez.github.io/irace/) package is used.
Tuning setups for individual problems are prepared in the permutator/tuning_setups/ directory.




## Gurobi benchmark

Some of the addressed problems were formulated as MILP and the permutator was compared with the Gurobi optimizer.
The experiment can be replicated using the following instructions.

TODO UPDATE

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




