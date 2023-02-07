#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/SUDOKU-ilp/$TIMESTAMP"

DATASETS=(
          "../data/SUDOKU/inst9x9_5_0.txt" "../data/SUDOKU/inst9x9_5_1.txt" "../data/SUDOKU/inst9x9_5_2.txt"
          "../data/SUDOKU/inst16x16_5_0.txt" "../data/SUDOKU/inst16x16_10_0.txt" "../data/SUDOKU/inst16x16_15_1.txt"
          "../data/SUDOKU/inst25x25_20_0.txt" "../data/SUDOKU/inst25x25_15_0.txt" "../data/SUDOKU/inst25x25_10_0.txt" "../data/SUDOKU/inst25x25_5_0.txt"
           )
TIMEOUTS=( 600 600 600 1800 1800 1800 3600 3600 3600 3600 )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"
ITER=0
for data in "${DATASETS[@]}"
do
  echo ${TIMEOUTS[${ITER}]}
  filename=$(basename $data ".txt")
  echo "[gurobi] Testing ${filename}..."
  $METAOPT_BIN/SUDOKU_ilp -d $data -t ${TIMEOUTS[${ITER}]} -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
  ITER=$ITER+1
done