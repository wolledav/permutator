#!/bin/bash
# Rename all *.txt to *.text
for f in *.txt; do 
    mv -- "$f" "${f%.txt}.dat"
done

