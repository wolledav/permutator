#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TIMESTAMP=$(date +'%Y%m%d-%H%M%S')
LOGDIR="../log/NPFS-ilp/$TIMESTAMP"

DATASETS=(
          "../data/NPFS-vfr/Small/VFR20_5_1.txt" "../data/NPFS-vfr/Small/VFR20_15_1.txt" "../data/NPFS-vfr/Small/VFR40_5_1.txt"
          "../data/NPFS-vfr/Large/VFR100_20_1.txt" "../data/NPFS-vfr/Large/VFR100_20_2.txt" "../data/NPFS-vfr/Large/VFR200_20_1.txt"
          "../data/NPFS-vfr/Large/VFR600_40_1.txt" "../data/NPFS-vfr/Large/VFR600_60_1.txt"
          "../data/NPFS-vfr/Large/VFR400_40_1.txt" "../data/NPFS-vfr/Large/VFR400_60_1.txt"
           )
#TIMEOUTS=( 600 600 600 1800 1800 1800 3600 3600 3600 3600 )
TIMEOUTS=( 6 6 6 6 6 6 6 6 6 6 )

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"
ITER=0
for data in "${DATASETS[@]}"
do
  echo ${TIMEOUTS[${ITER}]}
  filename=$(basename $data ".txt")
  echo "[gurobi] Testing ${filename}..."
  $METAOPT_BIN/NPFS_ilp -d $data -t ${TIMEOUTS[${ITER}]} -o "${LOGDIR}/ilp-${filename}.out" > "${LOGDIR}/ilp-${filename}.stdout" 2>&1
  ITER=$ITER+1
done