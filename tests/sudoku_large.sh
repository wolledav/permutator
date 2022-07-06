#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=3600 # 60 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/sudoku-large/$TIMESTAMP"
CONFIG="../configs/SUDOKU_final_1.json"

DATASETS=( "../data/sudoku/sudoku-10/inst25x25_20_0.txt" "../data/sudoku/sudoku-10/inst25x25_15_0.txt" "../data/sudoku/sudoku-10/inst25x25_10_0.txt" "../data/sudoku/sudoku-10/inst25x25_5_0.txt" )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".txt")
  for i in {1..50}
  do
    echo "[$i/50] Testing ${filename}..."
    $METAOPT_BIN/sudoku_meta -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/$i-meta-${filename}.out" > /dev/null 2>&1
  done
done