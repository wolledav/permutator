#!/bin/bash

CMD="./cmake-build-release/scp_meta_stable"
TEST_TIMEOUT=3
DATASET_DIR="./data/tspsd/"
LOG_DIR="./log/scp-meta/"


D_NAME="random_tsp24"

D_PATH="${DATASET_DIR}${D_NAME}/"
LOG_PATH="${LOG_DIR}${D_NAME}_${TEST_TIMEOUT}s"
cmake -E make_directory "$LOG_PATH"
for FILE in "$D_PATH"*
do
  FILENAME=$(basename $FILE ".json")
  for i in {1..1}
  do
    $CMD -d $FILE -t $TEST_TIMEOUT -o "${LOG_PATH}/${FILENAME}.out"
  done
done


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
#
#D_NAME="eil101_distance"
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
#
#D_NAME="ch150_distance"
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
#
#D_NAME="a280_distance"
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
