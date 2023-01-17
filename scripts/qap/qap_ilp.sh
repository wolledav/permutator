#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/qap-ilp/$TIMESTAMP"

DATASETS=( "../data/qap-taillard/tai20b.dat" "../data/qap-taillard/tai25a.dat" "../data/qap-taillard/tai30b.dat"
          "../data/qap-taillard/tai40a.dat" "../data/qap-taillard/tai50b.dat" "../data/qap-taillard/tai60a.dat"
          "../data/qap-taillard/tai80a.dat" "../data/qap-taillard/tai80b.dat" "../data/qap-taillard/tai100a.dat" "../data/qap-taillard/tai100b.dat" )
# TIMEOUTS=( 600 600 600 1800 1800 1800 3600 3600 3600 3600 )             # default time
TIMEOUTS=( 6000 6000 6000 18000 18000 18000 36000 36000 36000 36000 )   # default time x 10


echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"
ITER=0
for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".dat")
  echo "[gurobi] Testing ${filename}..."
  $METAOPT_BIN/qap_ilp -d $data -t ${TIMEOUTS[${ITER}]} -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
  ITER=$ITER+1
done