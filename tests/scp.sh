#!/bin/bash

TEST_TIMEOUT=600
CMD="./cmake-build-release/scp_meta"


DATASET=("./data/tspsd/berlin52_x_closest_edges/")
LOGDIR="./log/scp-meta/berlin52_x_closest_edges_10min/"

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


DATASET=("./data/tspsd/berlin52_distance/")
LOGDIR="./log/scp-meta/berlin52_distance_10min/"

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
