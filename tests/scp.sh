#!/bin/bash

CMD="./cmake-build-release/scp_meta_stable"
TEST_TIMEOUT=600
LOGDIR="./log/scp-meta/berlin52_random_10min/"
# CONFIG="../configs/.json" # default config used

DATASET=("./data/tspsd/berlin52_random/")

echo $LOGDIR
cmake -E make_directory "$LOGDIR"

for data in "$DATASET"*
do
  echo $data
  filename=$(basename $data ".json")
  for i in {1..1}
  do
    echo "[$i/1] Testing ${filename}..."
    $CMD -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}.out"
  done
done