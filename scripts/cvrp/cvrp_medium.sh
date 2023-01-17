#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=1800 # 30 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/cvrp-medium/$TIMESTAMP"
CONFIG="../configs/CVRP_final_3.json"

DATASETS=( "../data/cvrp/cvrp-10/X-n148-k46.xml" "../data/cvrp/cvrp-10/X-n204-k19.xml" "../data/cvrp/cvrp-10/X-n251-k28.xml" )

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