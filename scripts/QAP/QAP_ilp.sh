#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/QAP-ilp/$TIMESTAMP"

DATASETS=( "../data/QAP-taillard/tai20b.dat" "../data/QAP-taillard/tai25a.dat" "../data/QAP-taillard/tai30b.dat"
          "../data/QAP-taillard/tai40a.dat" "../data/QAP-taillard/tai50b.dat" "../data/QAP-taillard/tai60a.dat"
          "../data/QAP-taillard/tai80a.dat" "../data/QAP-taillard/tai80b.dat" "../data/QAP-taillard/tai100a.dat" "../data/QAP-taillard/tai100b.dat" )
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
  $METAOPT_BIN/QAP_ilp -d $data -t ${TIMEOUTS[${ITER}]} -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
  ITER=$ITER+1
done