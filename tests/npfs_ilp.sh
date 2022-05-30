#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/npfs-ilp/$TIMESTAMP"

DATASETS=(
          "../data/npfs-vfr/Small/VFR20_5_1_Gap.txt" "../data/npfs-vfr/Small/VFR20_15_1_Gap.txt" "../data/npfs-vfr/Small/VFR40_5_1_Gap.txt"
          "../data/npfs-vfr/Large/VFR100_20_1_Gap.txt" "../data/npfs-vfr/Large/VFR100_20_2_Gap.txt" "../data/npfs-vfr/Large/VFR200_20_1_Gap.txt"
          "../data/npfs-vfr/Large/VFR600_40_1_Gap.txt" "../data/npfs-vfr/Large/VFR600_60_1_Gap.txt"
          "../data/npfs-vfr/Large/VFR400_40_1_Gap.txt" "../data/npfs-vfr/Large/VFR400_60_1_Gap.txt"
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
  $METAOPT_BIN/npfs_ilp -d $data -t ${TIMEOUTS[${ITER}]} -o "${LOGDIR}/ilp-${filename}.out" #> /dev/null 2>&1
  ITER=$ITER+1
done