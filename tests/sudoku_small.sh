#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=600 # 10 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/sudoku-small/$TIMESTAMP"

DATASETS=( "../data/sudoku/inst9x9_5_0.txt" "../data/sudoku/inst9x9_5_1.txt" "../data/sudoku/inst9x9_5_2.txt" )

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