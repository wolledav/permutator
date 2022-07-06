#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=3600 # 60 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/npfs-large/$TIMESTAMP"
CONFIG="../configs/NPFS_final_1.json"

DATASETS=( "../data/npfs-vfr/npfs-10/VFR400_40_1.txt" "../data/npfs-vfr/npfs-10/VFR400_60_1.txt" "../data/npfs-vfr/npfs-10/VFR600_40_1.txt" "../data/npfs-vfr/npfs-10/VFR600_60_1.txt")

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
    $METAOPT_BIN/npfs_meta -d $data -c $CONFIG -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}/$i-meta-${filename}.out"
  done
#  $METAOPT_BIN/npfs_ilp -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
done