#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=60 # 1 minute
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/tspsd/$TIMESTAMP"
# CONFIG="../configs/.json" # default config used

DATASET=("../data/tspsd/berlin52_random/")

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "$DATASET"/*
do
  echo $data
  filename=$(basename $data ".json")
  for i in {1..1}
  do
    echo "[$i/1] Testing ${filename}..."
    $METAOPT_BIN/tspsd_meta -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}.out"
  done
done