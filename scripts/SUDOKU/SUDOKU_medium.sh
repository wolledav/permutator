#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=1800 # 30 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/SUDOKU-medium/$TIMESTAMP"
CONFIG="../configs/SUDOKU_final_1.json"

DATASETS=( "../data/SUDOKU/SUDOKU-10/inst16x16_5_0.txt" "../data/SUDOKU/SUDOKU-10/inst16x16_10_0.txt" "../data/SUDOKU/SUDOKU-10/inst16x16_15_1.txt" )

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
    $METAOPT_BIN/SUDOKU_meta -d $data -c $CONFIG -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}/$i-meta-${filename}.out"
  done
done