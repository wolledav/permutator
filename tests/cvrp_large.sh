#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=3600 # 60 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/cvrp-large/$TIMESTAMP"

DATASETS=( "../data/cvrp-uchoa/X-n351-k40.xml" "../data/cvrp-uchoa/X-n561-k42.xml" "../data/cvrp-uchoa/X-n749-k98.xml" "../data/cvrp-uchoa/X-n1001-k43.xml" )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".xml")
  for i in {1..50}
  do
    echo "[$i/50] Testing ${filename}..."
    $METAOPT_BIN/cvrp_meta -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/$i-meta-${filename}.out" > /dev/null 2>&1
  done
  $METAOPT_BIN/cvrp_ilp -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
done