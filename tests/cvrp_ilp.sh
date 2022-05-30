#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/cvrp-ilp/$TIMESTAMP"

DATASETS=( "../data/cvrp-augerat-A/A-n65-k09.xml" "../data/cvrp-augerat-P/P-n050-k10.xml" "../data/cvrp-augerat-P/P-n076-k05.xml"
          "../data/cvrp-uchoa/X-n148-k46.xml" "../data/cvrp-uchoa/X-n204-k19.xml" "../data/cvrp-uchoa/X-n251-k28.xml"
          "../data/cvrp-uchoa/X-n351-k40.xml" "../data/cvrp-uchoa/X-n561-k42.xml" "../data/cvrp-uchoa/X-n749-k98.xml" "../data/cvrp-uchoa/X-n1001-k43.xml" )
TIMEOUTS=( 600 600 600 1800 1800 1800 3600 3600 3600 3600 )
#TIMEOUTS=( 60 60 60 60 60 60 60 60 60 60 )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"
ITER=0
for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".xml")
  echo "[gurobi] Testing ${filename}..."
  $METAOPT_BIN/cvrp_ilp -d $data -t ${TIMEOUTS[${ITER}]} -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
  ITER=$ITER+1
done