#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=1800 # 30 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/npfs-medium/$TIMESTAMP"

DATASETS=( "../data/npfs-vfr/Large/VFR100_20_1.txt" "../data/npfs-vfr/Large/VFR100_20_2.txt" "../data/npfs-vfr/Large/VFR200_20_1.txt" )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".xml")
  for i in {1..50}
  do
    echo "[$i/50] Testing ${filename}..."
    $METAOPT_BIN/npfs_meta -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/$i-meta-${filename}.out" > /dev/null 2>&1
  done
  $METAOPT_BIN/npfs_ilp -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
done