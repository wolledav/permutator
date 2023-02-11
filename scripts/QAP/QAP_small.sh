#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=600 # 10 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/QAP-small/$TIMESTAMP"
CONFIG="../configs/QAP_final_3.json"

DATASETS=( "../data/QAP/QAP-10/tai20b.dat" "../data/QAP/QAP-10/tai25a.dat" "../data/QAP/QAP-10/tai30b.dat" )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".dat")
  for i in {1..50}
  do
    echo "[$i/50] Testing ${filename}..."
    cmake -E make_directory "$LOGDIR"/"$filename"
    $METAOPT_BIN/QAP_meta -d $data -c $CONFIG -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}/$i-meta-${filename}.out"
  done
done