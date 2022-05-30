#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=1800 # 30 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/sudoku-medium/$TIMESTAMP"

DATASETS=( "../data/sudoku/inst16x16_5_0.txt" "../data/sudoku/inst16x16_10_0.txt" "../data/sudoku/inst16x16_15_1.txt" )

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