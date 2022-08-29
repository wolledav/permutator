#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=600

LOGDIR="../log/wcp-meta/berlin52_closest"
# CONFIG="../configs/.json" # default config used
DATASET=("../data/tspsd/berlin52_closest/")

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "$DATASET"*
do
  echo $data
  filename=$(basename $data ".json")
  for i in {1..1}
  do
    echo "[$i/1] Testing ${filename}..."
    $METAOPT_BIN/scp_meta -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}.out"
  done
done

LOGDIR="../log/wcp-meta/berlin52_random"
# CONFIG="../configs/.json" # default config used
DATASET=("../data/tspsd/berlin52_random/")

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "$DATASET"*
do
  echo $data
  filename=$(basename $data ".json")
  for i in {1..1}
  do
    echo "[$i/1] Testing ${filename}..."
    $METAOPT_BIN/scp_meta -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}.out"
  done
done