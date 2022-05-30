#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=600 # 10 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/qap-small/$TIMESTAMP"

DATASETS=( "../data/qap-taillard/tai20b.dat" "../data/qap-taillard/tai25a.dat" "../data/qap-taillard/tai30b.dat" )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".dat")
  for i in {1..50}
  do
    echo "[$i/50] Testing ${filename}..."
    $METAOPT_BIN/qap_meta -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/$i-meta-${filename}.out" > /dev/null 2>&1
  done
done