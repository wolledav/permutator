#!/bin/bash

./tests/wcp.sh
./tests/scp.sh

# SCP, large instance, 60 minutes
#INSTANCE="./data/tspsd/vm1084_closest/vm1084_sd0_cl.json"
#TIMEOUT=36000
#
#CMD="./cmake-build-release/scp_meta_stable_log"
#FILENAME=$(basename $INSTANCE ".json")
#OUT_DIR="./log/scp-meta/vm1084_600min/"
#LOG="${OUT_DIR}vm1084_sd0_cl.log"
#
#cmake -E make_directory "$OUT_DIR"
#$CMD -d $INSTANCE -o "${OUT_DIR}/${FILENAME}.out" -t $TIMEOUT >> $LOG
#
#
## SCP, large instance, 60 minutes
#INSTANCE="./data/tspsd/vm1084_closest/vm1084_sd1_cl.json"
#TIMEOUT=36000
#
#CMD="./cmake-build-release/scp_meta_stable_log"
#FILENAME=$(basename $INSTANCE ".json")
#OUT_DIR="./log/scp-meta/vm1084_600min/"
#LOG="${OUT_DIR}vm1084_sd1_cl.log"
#
#cmake -E make_directory "$OUT_DIR"
#$CMD -d $INSTANCE -o "${OUT_DIR}/${FILENAME}.out" -t $TIMEOUT >> $LOG
#
#
## WCP, large instance, 60 minutes
#INSTANCE="./data/tspsd/vm1084_closest/vm1084_sd3_cl.json"
#TIMEOUT=36000
#
#CMD="./cmake-build-release/wcp_meta_stable_log"
#FILENAME=$(basename $INSTANCE ".json")
#OUT_DIR="./log/wcp-meta/vm1084_600min/"
#LOG="${OUT_DIR}vm1084_sd3_cl.log"
#
#cmake -E make_directory "$OUT_DIR"
#$CMD -d $INSTANCE -o "${OUT_DIR}/${FILENAME}.out" -t $TIMEOUT >> $LOG
#
#
## SCP, large instance, 60 minutes
#INSTANCE="./data/tspsd/vm1084_closest/vm1084_sd5_cl.json"
#TIMEOUT=36000
#
#CMD="./cmake-build-release/scp_meta_stable_log"
#FILENAME=$(basename $INSTANCE ".json")
#OUT_DIR="./log/scp-meta/vm1084_600min/"
#LOG="${OUT_DIR}vm1084_sd5_cl.log"
#
#cmake -E make_directory "$OUT_DIR"
#$CMD -d $INSTANCE -o "${OUT_DIR}/${FILENAME}.out" -t $TIMEOUT >> $LOG

# berlin52_random_10min
#./tests/scp.sh
