#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Invalid number of params. Usage:"
    echo "./qap_single.sh [timeout] [instance file]"
    exit 1
fi

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=$1
LOGDIR="../log/qap-fix"
INSTANCE=$2

echo $LOGDIR
# Create log directory
cmake -E make_directory "$LOGDIR"

filename=$(basename $INSTANCE ".dat")
for i in {1..50}
do
  echo "[$i/50] Testing ${filename}..."
  $METAOPT_BIN/qap_meta -d $INSTANCE -t $TEST_TIMEOUT -o "${LOGDIR}/$i-meta-${filename}.out"
done