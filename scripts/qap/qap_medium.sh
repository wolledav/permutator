#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=1800 # 30 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/qap-medium/$TIMESTAMP"
CONFIG="../configs/QAP_final_3.json"

DATASETS=( "../data/qap/qap-10/tai40a.dat" "../data/qap/qap-10/tai50b.dat" "../data/qap/qap-10/tai60a.dat" )

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
    $METAOPT_BIN/qap_meta -d $data -c $CONFIG -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}/$i-meta-${filename}.out"
  done
done