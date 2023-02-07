#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=600 # 10 minues
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/CVRP-small/$TIMESTAMP"
CONFIG="../configs/CVRP_final_3.json"

DATASETS=( "../data/CVRP/CVRP-10/A-n65-k09.xml" "../data/CVRP/CVRP-10/P-n050-k10.xml" "../data/CVRP/CVRP-10/P-n076-k05.xml" )

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
    $METAOPT_BIN/CVRP_meta -d $data -c $CONFIG -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}/$i-meta-${filename}.out"
  done
#  $METAOPT_BIN/CVRP_ilp -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
done
