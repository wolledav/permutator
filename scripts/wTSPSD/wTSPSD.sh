#!/bin/bash

CMD="./cmake-build-release/wTSPSD_meta_stable"
TEST_TIMEOUT=3
DATASET_DIR="./data/tspsd/"
LOG_DIR="./log/wTSPSD-meta/"


D_NAME="random24_ov_100"

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


## berlin52_closest with warm starting
#CMD="./cmake-build-release/wTSPSD_meta_stable"
#TEST_TIMEOUT=600
#DATASET_DIR="./data/tspsd/"
#LOG_DIR="./log/wTSPSD-meta/"
#
#D_NAME="berlin52_closest"
#INIT_DIR="./log/TSPSD-meta/berlin52_closest_10min/"
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

#D_NAME="eil101_distance"
#INIT_DIR="./log/TSPSD-meta/eil101_distance_3600s/"
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
#INIT_DIR="./log/TSPSD-meta/ch150_distance_3600s/"
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
#INIT_DIR="./log/TSPSD-meta/a280_distance_3600s/"
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
#INIT_DIR="./log/TSPSD-meta/vm1084_600min/"
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
