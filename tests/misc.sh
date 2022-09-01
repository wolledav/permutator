#!/bin/bash

INSTANCE="./data/tspsd/vm1084_closest/vm1084_sd3_cl.json"
TIMEOUT=3600

# SCP, large instance, 60 minutes
CMD="./cmake-build-release/scp_meta_stable_log"
FILENAME=$(basename $INSTANCE ".json")
OUT_DIR="./log/scp-meta/vm1084_60min/"
LOG="${OUT_DIR}vm1084_sd3_cl.log"

cmake -E make_directory "$OUT_DIR"
$CMD -d $INSTANCE -o "${OUT_DIR}/${FILENAME}.out" -t $TIMEOUT >> $LOG

# berlin52_random_10min
./tests/scp.sh

# WCP, large instance, 60 minutes
CMD="./cmake-build-release/wcp_meta_stable_log"
FILENAME=$(basename $INSTANCE ".json")
OUT_DIR="./log/wcp-meta/vm1084_60min/"
LOG="${OUT_DIR}vm1084_sd3_cl.log"

cmake -E make_directory "$OUT_DIR"
$CMD -d $INSTANCE -o "${OUT_DIR}/${FILENAME}.out" -t $TIMEOUT >> $LOG
