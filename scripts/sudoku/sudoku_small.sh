#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=600 # 10 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/sudoku-small/$TIMESTAMP"
CONFIG="../configs/SUDOKU_final_1.json"

DATASETS=( "../data/sudoku/sudoku-10/inst9x9_5_0.txt" "../data/sudoku/sudoku-10/inst9x9_5_1.txt" "../data/sudoku/sudoku-10/inst9x9_5_2.txt" )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".txt")
  for i in {1..50}
  do
    echo "[$i/50] Testing ${filename}..."
    cmake -E make_directory "$LOGDIR"/"$filename"
    $METAOPT_BIN/sudoku_meta -d $data  -c $CONFIG -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}/$i-meta-${filename}.out"
  done
done