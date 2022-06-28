#!/bin/bash

METAOPT_BIN="../cmake-build-release"
TEST_TIMEOUT=3

echo "Testing Capacitated Vehicle Routing Problem.."

echo -n -e "CVRP Metaheuristic: \t"
if $METAOPT_BIN/cvrp_meta -d "../data/cvrp-augerat-P/P-n016-k08.xml" -t $TEST_TIMEOUT > /dev/null 2>&1; then
  echo "[OK]"
else
  echo "[FAILED]"
fi

echo -n -e "CVRP MILP: \t\t"
if $METAOPT_BIN/cvrp_ilp -d "../data/cvrp-augerat-P/P-n016-k08.xml" -t $TEST_TIMEOUT > /dev/null 2>&1; then
  echo "[OK]"
else
  echo "[FAILED]"
fi

echo "Testing Qadratic Assignment Problem..."

echo -n -e "QAP Metaheuristic: \t"
if $METAOPT_BIN/qap_meta -d "../data/qap-taillard/tai5a.txt" -t $TEST_TIMEOUT > /dev/null 2>&1; then
  echo "[OK]"
else
  echo "[FAILED]"
fi

echo -n -e "QAP MILP: \t\t"
if $METAOPT_BIN/qap_ilp -d "../data/qap-taillard/tai5a.txt" -t $TEST_TIMEOUT > /dev/null 2>&1; then
  echo "[OK]"
else
  echo "[FAILED]"
fi

echo "Testing Non-permutation Flowshop Scheduling Problem..."

echo -n -e "NPFS Metaheuristic: \t"
if $METAOPT_BIN/npfs_meta -d "../data/npfs-vfr/Small/VFR10_5_1.txt" -t $TEST_TIMEOUT > /dev/null 2>&1; then
  echo "[OK]"
else
  echo "[FAILED]"
fi

echo -n -e "NPFS MILP: \t\t"
if $METAOPT_BIN/npfs_ilp -d "../data/npfs-vfr/Small/VFR10_5_1.txt" -t $TEST_TIMEOUT > /dev/null 2>&1; then
  echo "[OK]"
else
  echo "[FAILED]"
fi