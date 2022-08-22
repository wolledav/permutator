#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=1800 # 30 minutes
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/npfs-medium/$TIMESTAMP"
CONFIG="../configs/NPFS_final_2.json"

DATASETS=( "../data/npfs-vfr/npfs-10/VFR100_20_1.txt" "../data/npfs-vfr/npfs-10/VFR100_20_2.txt" "../data/npfs-vfr/npfs-10/VFR200_20_1.txt" )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

for data in "${DATASETS[@]}"
do
  filename=$(basename $data ".xml")
  for i in {1..50}
  do
    echo "[$i/50] Testing ${filename}..."
    cmake -E make_directory "$LOGDIR"/"$filename"
    $METAOPT_BIN/npfs_meta -d $data -c $CONFIG -t $TEST_TIMEOUT -o "${LOGDIR}/${filename}/$i-meta-${filename}.out"
  done
#  $METAOPT_BIN/npfs_ilp -d $data -t $TEST_TIMEOUT -o "${LOGDIR}/ilp-${filename}.out" > /dev/null 2>&1
done