#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=600 # 10 minues
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/cvrp-small/$TIMESTAMP"
CONFIG="../configs/cvrp_final_2.json"

DATASETS=( "../data/cvrp/cvrp-10/A-n65-k09.xml" "../data/cvrp/cvrp-10/P-n050-k10.xml" "../data/cvrp/cvrp-10/P-n076-k05.xml" )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".xml")
  for i in {1..50}
  do
    echo "[$i/50] Testing ${filename}..."
    cmake -E make_directory "$LOGDIR"/"$filename"
    $METAOPT_BIN/cvrp_meta -d $data -c $CONFIG -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}/$i-meta-${filename}.out"
  done
#  $METAOPT_BIN/cvrp_ilp -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
done
