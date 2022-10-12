#!/bin/bash

CMD="./cmake-build-release/wcp_meta_stable"
TEST_TIMEOUT=600
DATASET_DIR="./data/tspsd/"
LOG_DIR="./log/wcp-meta/"

D_NAME="berlin52_closest"
INIT_DIR="./log/scp-meta/berlin52_closest_10min/"

D_PATH="${DATASET_DIR}${D_NAME}/"
LOG_PATH="${LOG_DIR}${D_NAME}_${TEST_TIMEOUT}s_ws"
cmake -E make_directory "$LOG_PATH"
for FILE in "$D_PATH"*
do
  FILENAME=$(basename $FILE ".json")
  INIT_PATH="${INIT_DIR}${FILENAME}.out"
  for i in {1..1}
  do
    $CMD -d $FILE -t $TEST_TIMEOUT -o "${LOG_PATH}/${FILENAME}.out" -i $INIT_PATH
  done
done

#D_NAME="eil101_distance"
#INIT_DIR="./log/scp-meta/eil101_distance_3600s/"
#
#D_PATH="${DATASET_DIR}${D_NAME}/"
#LOG_PATH="${LOG_DIR}${D_NAME}_${TEST_TIMEOUT}s_ws"
#cmake -E make_directory "$LOG_PATH"
#for FILE in "$D_PATH"*
#do
#  FILENAME=$(basename $FILE ".json")
#  INIT_PATH="${INIT_DIR}${FILENAME}.out"
#  for i in {1..1}
#  do
#    $CMD -d $FILE -t $TEST_TIMEOUT -o "${LOG_PATH}/${FILENAME}.out" -i $INIT_PATH
#  done
#done
#
#D_NAME="ch150_distance"
#INIT_DIR="./log/scp-meta/ch150_distance_3600s/"
#
#D_PATH="${DATASET_DIR}${D_NAME}/"
#LOG_PATH="${LOG_DIR}${D_NAME}_${TEST_TIMEOUT}s_ws"
#cmake -E make_directory "$LOG_PATH"
#for FILE in "$D_PATH"*
#do
#  FILENAME=$(basename $FILE ".json")
#  INIT_PATH="${INIT_DIR}${FILENAME}.out"
#  for i in {1..1}
#  do
#    $CMD -d $FILE -t $TEST_TIMEOUT -o "${LOG_PATH}/${FILENAME}.out" -i $INIT_PATH
#  done
#done
#
#D_NAME="a280_distance"
#INIT_DIR="./log/scp-meta/a280_distance_3600s/"
#
#D_PATH="${DATASET_DIR}${D_NAME}/"
#LOG_PATH="${LOG_DIR}${D_NAME}_${TEST_TIMEOUT}s_ws"
#cmake -E make_directory "$LOG_PATH"
#for FILE in "$D_PATH"*
#do
#  FILENAME=$(basename $FILE ".json")
#  INIT_PATH="${INIT_DIR}${FILENAME}.out"
#  for i in {1..1}
#  do
#    $CMD -d $FILE -t $TEST_TIMEOUT -o "${LOG_PATH}/${FILENAME}.out" -i $INIT_PATH
#  done
#done
#
#D_NAME="vm1084_closest"
#INIT_DIR="./log/scp-meta/vm1084_600min/"
#
#D_PATH="${DATASET_DIR}${D_NAME}/"
#LOG_PATH="${LOG_DIR}${D_NAME}_${TEST_TIMEOUT}s_ws"
#cmake -E make_directory "$LOG_PATH"
#for FILE in "$D_PATH"*
#do
#  FILENAME=$(basename $FILE ".json")
#  INIT_PATH="${INIT_DIR}${FILENAME}.out"
#  for i in {1..1}
#  do
#    $CMD -d $FILE -t $TEST_TIMEOUT -o "${LOG_PATH}/${FILENAME}.out" -i $INIT_PATH
#  done
#done


#D_NAME="berlin52_distance_v2"
#
#D_PATH="${DATASET_DIR}${D_NAME}/"
#LOG_PATH="${LOG_DIR}${D_NAME}_${TEST_TIMEOUT}s"
#cmake -E make_directory "$LOG_PATH"
#for FILE in "$D_PATH"*
#do
#  FILENAME=$(basename $FILE ".json")
#  for i in {1..1}
#  do
#    $CMD -d $FILE -t $TEST_TIMEOUT -o "${LOG_PATH}/${FILENAME}.out"
#  done
#done
