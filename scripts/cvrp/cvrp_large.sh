#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=3600 # 60 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/cvrp-large/$TIMESTAMP"
CONFIG="../configs/CVRP_final_3.json"

DATASETS=( "../data/cvrp/cvrp-10/X-n351-k40.xml" "../data/cvrp/cvrp-10/X-n561-k42.xml" "../data/cvrp/cvrp-10/X-n749-k98.xml" "../data/cvrp/cvrp-10/X-n1001-k43.xml" )

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